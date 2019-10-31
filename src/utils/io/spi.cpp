/*
 * Authors: M. Kristien
 * Organisation: HYPED
 * Date: 18. April 2018
 *
 *    Copyright 2018 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licen ses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */


#include "utils/io/spi.hpp"

// #include <stdio.h>
#include <poll.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>

#ifndef WIN
#include <linux/spi/spidev.h>
#else
#define _IOW(type, nr, size) 10   // random demo functionality
#define SPI_IOC_MAGIC             'k'
#define SPI_IOC_WR_MODE           _IOW(SPI_IOC_MAGIC, 1, uint8_t)
#define SPI_IOC_WR_MAX_SPEED_HZ   _IOW(SPI_IOC_MAGIC, 4, uint32_t)
#define SPI_IOC_WR_LSB_FIRST      _IOW(SPI_IOC_MAGIC, 2, uint8_t)
#define SPI_IOC_WR_BITS_PER_WORD  _IOW(SPI_IOC_MAGIC, 3, uint8_t)
struct spi_ioc_transfer {
  uint64_t tx_buf;
  uint64_t rx_buf;

  uint32_t len;
  uint32_t speed_hz;

  uint16_t delay_usecs;
  uint8_t  bits_per_word;
  uint8_t  cs_change;
  uint8_t  tx_nbits;
  uint8_t  rx_nbits;
  uint16_t pad;
};
#define SPI_MSGSIZE(N) \
  ((((N)*(sizeof(struct spi_ioc_transfer))) < (1 << _IOC_SIZEBITS)) \
    ? ((N)*(sizeof(struct spi_ioc_transfer))) : 0)
#define SPI_IOC_MESSAGE(N)  _IOW(SPI_IOC_MAGIC, 0, char[SPI_MSGSIZE(N)])
#define SPI_CS_HIGH         0x04
#endif  // ifndef WIN

#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"

// configure SPI
#define SPI_MODE  3
#define SPI_BITS  8         // each word is 1B
#define SPI_MSBFIRST 0
#define SPI_LSBFIRST 1

#define SPI_FS  0


namespace hyped {
namespace utils {
namespace io {

constexpr uint32_t kSPIAddrBase = 0x481A0000;   // 0x48030000 for SPI0
constexpr uint32_t kMmapSize    = 0x1000;

// define what the address space of SPI looks like
#pragma pack(1)
struct SPI_CH {   // offset
  uint32_t conf;  // 0x00
  uint32_t stat;  // 0x04
  uint32_t ctrl;  // 0x08
  uint32_t tx;    // 0x0c
  uint32_t rx;    // 0x10
};

#pragma pack(1)    // so that the compiler does not change layout
struct SPI_HW {           // offset
  uint32_t revision;      // 0x000
  uint32_t nope0[0x43];   // 0x004 - 0x110
  uint32_t sysconfig;     // 0x110
  uint32_t sysstatus;     // 0x114
  uint32_t irqstatus;     // 0x118
  uint32_t irqenable;     // 0x11c
  uint32_t nope1[2];      // 0x120 - 0x124
  uint32_t syst;          // 0x124
  uint32_t modulctr;      // 0x128
  SPI_CH   ch0;           // 0x12c - 0x140
  SPI_CH   ch1;           // 0x140 - 0x154
  SPI_CH   ch2;           // 0x154 - 0x168
  SPI_CH   ch3;           // 0x168 - 0x17c
  uint32_t xferlevel;     // 0x17c
};

SPI& SPI::getInstance()
{
  static SPI spi(System::getLogger());
  return spi;
}

SPI::SPI(Logger& log)
    : spi_fd_(-1),
      hw_(0),
      ch_(0),
      log_(log)
{
  const char device[] = "/dev/spidev2.0";   // spidev1.0 for SPI0
  spi_fd_ = open(device, O_RDWR, 0);

  if (spi_fd_ < 0) {
    log_.ERR("SPI", "Could not open spi device");
    return;
  }

  // set clock frequency
  setClock(Clock::k1MHz);

  uint8_t bits = SPI_BITS;      // need to change this value
  if (ioctl(spi_fd_, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
    log_.ERR("SPI", "could not set bits per word");
  }

  // set clock mode and CS active low
  uint8_t mode = (SPI_MODE & 0x3) & ~SPI_CS_HIGH;
  if (ioctl(spi_fd_, SPI_IOC_WR_MODE, &mode) < 0) {
    log_.ERR("SPI", "could not set mode");
  }

  // set bit order
  uint8_t order = SPI_MSBFIRST;
  if (ioctl(spi_fd_, SPI_IOC_WR_LSB_FIRST, &order) < 0) {
    log_.ERR("SPI", "could not set bit order");
  }

  bool check_init = initialise();
  if (check_init) {
    log_.INFO("SPI", "spi instance created");
  } else {
    log_.ERR("SPI", "spi instansiation failed");
  }
}

bool SPI::initialise()
{
  int   fd;
  void* base;

  fd = open("/dev/mem", O_RDWR);
  if (fd < 0) {
    log_.ERR("SPI", "could not open /dev/mem");
    return false;
  }

  base = mmap(0, kMmapSize, PROT_READ | PROT_WRITE, MAP_SHARED,
              fd, kSPIAddrBase);
  if (base == MAP_FAILED) {
    log_.ERR("SPI", "could not map bank 0x%x", kSPIAddrBase);
    return false;
  }

  hw_ = reinterpret_cast<SPI_HW*>(base);
  ch_ = &hw_->ch0;

  log_.INFO("SPI", "Mapping successfully created %d", sizeof(SPI_HW));
  log_.INFO("SPI", "revision 0x%x", hw_->revision);
  return true;
}


void SPI::setClock(Clock clk)
{
  uint32_t data;
  switch (clk) {
    case Clock::k1MHz:  data = 1000000;   break;
    case Clock::k4MHz:  data = 4000000;   break;
    case Clock::k16MHz: data = 16000000;  break;
    case Clock::k20MHz: data = 20000000;  break;
  }

  if (ioctl(spi_fd_, SPI_IOC_WR_MAX_SPEED_HZ, &data) < 0) {
    log_.ERR("SPI", "could not set clock frequency of %d", data);
  }
}

void SPI::transfer(uint8_t* tx, uint8_t* rx, uint16_t len)
{
#if SPI_FS
  if (spi_fd_ < 0) return;  // early exit if no spi device present
  spi_ioc_transfer message = {};

  message.tx_buf = reinterpret_cast<uint64_t>(tx);
  message.rx_buf = reinterpret_cast<uint64_t>(rx);
  message.len    = len;

  if (ioctl(spi_fd_, SPI_IOC_MESSAGE(1), &message) < 0) {
    log_.ERR("SPI", "could not submit TRANSFER message");
  }
#else

  if (hw_ == 0) return;   // early exit if no spi mapped

  for (uint16_t x = 0; x < len; x++) {
    // log_.INFO("SPI_TEST","channel 0 status before: %d", 10);
    // while(!(ch0->status & 0x2));
    log_.INFO("SPI_TEST", "Status register: %x", ch_->stat);
    ch_->ctrl = ch_->ctrl | 0x1;
    ch_->conf = ch_->conf & 0xfffcffff;
    ch_->tx = tx[x];
    log_.INFO("SPI_TEST", "Status register: %x", ch_->stat);
    log_.INFO("SPI_TEST", "Config register: %x", ch_->conf);
    log_.INFO("SPI_TEST", "Control register: %x", ch_->ctrl);

    while (!(ch_->stat & 0x1)) {
      utils::concurrent::Thread::sleep(1000);
      log_.INFO("SPI_TEST", "Status register: %d", ch_->stat);
    }
    log_.INFO("SPI_TEST", "Status register: %d", ch_->stat);
    // log_.INFO("SPI_TEST","Read buffer: %d", ch0->rx_buf);
    // log_.INFO("SPI_TEST","channel 0 status after: %d", 10);
    // write_buffer++;
  }

#endif
}

void SPI::read(uint8_t addr, uint8_t* rx, uint16_t len)
{
  if (spi_fd_ < 0) return;  // early exit if no spi device present

  spi_ioc_transfer message[2] = {};

  // send address
  message[0].tx_buf = reinterpret_cast<uint64_t>(&addr);
  message[0].rx_buf = 0;
  message[0].len    = 1;

  // receive data
  message[1].tx_buf = 0;
  message[1].rx_buf = reinterpret_cast<uint64_t>(rx);
  message[1].len    = len;

  if (ioctl(spi_fd_, SPI_IOC_MESSAGE(2), message) < 0) {
    log_.ERR("SPI", "could not submit 2 TRANSFER messages");
  }
}

void SPI::write(uint8_t addr, uint8_t* tx, uint16_t len)
{
  if (spi_fd_ < 0) return;  // early exit if no spi device present

  spi_ioc_transfer message[2] = {};
  // send address
  message[0].tx_buf = reinterpret_cast<uint64_t>(&addr);
  message[0].rx_buf = 0;
  message[0].len    = 1;

  // write data
  message[1].tx_buf = reinterpret_cast<uint64_t>(tx);
  message[1].rx_buf = 0;
  message[1].len    = len;

  if (ioctl(spi_fd_, SPI_IOC_MESSAGE(2), message) < 0) {
    log_.ERR("SPI", "could not submit 2 TRANSFER messages");
  }
}

SPI::~SPI()
{
  if (spi_fd_ < 0) return;  // early exit if no spi device present

  close(spi_fd_);
}
}}}   // namespace hyped::utils::io
