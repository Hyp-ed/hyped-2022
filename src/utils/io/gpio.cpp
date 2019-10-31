/*
 * Authors: Martin Kristien
 * Organisation: HYPED
 * Date: 20. April 2018
 * Description:
 *
 *    Copyright 2018 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
*/

#include "utils/io/gpio.hpp"

#include <poll.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
//
#include <iostream>
#include <fstream>
//
#include <cstdlib>
#include <cstring>

#include "utils/logger.hpp"
#include "utils/system.hpp"

namespace hyped {
namespace utils {
namespace io {

namespace gpio {
constexpr const off_t bases[kBankNum] = {
  0x44e07000,
  0x4804c000,
  0x481ac000,
  0x481ae000
};
constexpr uint32_t kMmapSize = 0x1000;

// register offsets
// constexpr uint32_t kOutputEnable  = 0x134;
constexpr uint32_t kData          = 0x138;
constexpr uint32_t kClear         = 0x190;
constexpr uint32_t kSet           = 0x194;

#define GPIOFS 0             // used to swtich to file system method for set(), clear(), and read()

// workaround to avoid conflict with GPIO::read()
uint8_t readHelper(int fd)
{
  char buf[2];
  lseek(fd, 0, SEEK_SET);                      // reset file pointer
  read(fd, buf, sizeof(buf));                  // actually consume new data, changes value in buffer
  return std::atoi(buf);
}

}  // namespace gpio

bool GPIO::initialised_ = false;
void* GPIO::base_mapping_[gpio::kBankNum];
std::vector<uint32_t> GPIO::exported_pins;              // NOLINT [build/include_what_you_use]

GPIO::GPIO(uint32_t pin, gpio::Direction direction)
    : GPIO(pin, direction, System::getLogger())
{ /* EMPTY, delegate to the other constructor */ }

GPIO::GPIO(uint32_t pin, gpio::Direction direction, Logger& log)
    : pin_(pin),
      direction_(direction),
      log_(log),
      set_(0),
      clear_(0),
      data_(0),
      fd_(0)
{
  if (!initialised_)  initialise();

  // check pin is not already allocated
  for (uint32_t pin : exported_pins) {
    if (pin_ == pin) {
      log_.ERR("GPIO", "pin %d already in use", pin_);
      return;
    }
  }
  exported_pins.push_back(pin_);

  exportGPIO();
  attachGPIO();
  if (direction == gpio::Direction::kOut) {              // sets pin value to 1 if direction is out
    set();
  }
}

void GPIO::initialise()
{
  int   fd;                                              // file descriptor
  off_t offset;
  void* base;
  Logger log(false, -1);

  fd = open("/dev/mem", O_RDWR);
  if (fd < 0) {
    log.ERR("GPIO", "could not open /dev/mem");
    return;
  }

  for (int i = 0; i < gpio::kBankNum; i++) {
    offset = gpio::bases[i];
    /**
     * @brief mmap() creates a new mapping in the virtual address space of the calling process
        The starting address is zero
        The length argument is kMmapSize
     * void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
     */
    base = mmap(0, gpio::kMmapSize, PROT_READ | PROT_WRITE, MAP_SHARED,
                fd, offset);
    if (base == MAP_FAILED) {
      log.ERR("GPIO", "could not map bank %d", offset);
      return;
    }

    base_mapping_[i] = base;
  }
  atexit(uninitialise);
  initialised_ = true;
}

void GPIO::uninitialise()
{
  Logger log(1, 1);
  log.ERR("GPIO", "uninitialising");

  // release exported gpios
  int fd;
  fd = open("/sys/class/gpio/unexport", O_WRONLY);
  if (fd < 0) {
    log.ERR("GPIO", "could not open unexport");
    return;
  }

  char buf[10];
  for (uint32_t pin : exported_pins) {
    snprintf(buf, sizeof(buf), "%d", pin);
    write(fd, buf, strlen(buf) + 1);
  }
  close(fd);
}

void GPIO::exportGPIO()
{
  if (!initialised_) {
    log_.ERR("GPIO", "servise has not been initialised");
    return;
  }

  char buf[100];
  log_.INFO("GPIO", "exporting %d", pin_);

  // let the kernel know we are using this pin
  int      fd;
  uint32_t len;
  fd = open("/sys/class/gpio/export", O_WRONLY);
  if (fd < 0) {
    log_.ERR("GPIO", "could not open export file");
    return;
  }
  snprintf(buf, sizeof(buf), "%d", pin_);
  len = write(fd, buf, strlen(buf) + 1);
  close(fd);
  if (len != strlen(buf) +1) {
    log_.INFO("GPIO", "could not export GPIO %d, might be already exported", pin_);
    // return;
  }

  // set direction
  snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%i/direction", pin_);
  fd = open(buf, O_WRONLY);
  if (fd < 0) {
    log_.ERR("GPIO", "could not open direction file for %i at %s", pin_, buf);
    return;
  }
  switch (direction_) {
    case gpio::Direction::kIn:
      len = write(fd, "in", 3);
      break;
    case gpio::Direction::kOut:
      len = write(fd, "out", 4);
      break;
  }
  close(fd);
  if (len < 3) {
    log_.ERR("GPIO", "could not set direction for %i", pin_);
    return;
  } else {
    log_.INFO("GPIO", "pin %d was successfully exported", pin_);
  }
  return;
}

void GPIO::attachGPIO()
{
  uint8_t bank;                                         // offset: GPIO_0,1,2,3
  uint8_t pin_id;

  bank      = pin_/32;
  pin_id    = pin_%32;
  // corresponds to desired data of pin by indicating specific bit within byte of pin data
  pin_mask_ = 1 << pin_id;
  log_.DBG1("GPIO", "gpio %d resolved as bank,pin %d, %d", pin_, bank, pin_id);

  // hacking compilation compatibility for 64bit systems
#ifdef ARCH_64
  uint64_t base = reinterpret_cast<uint64_t>(base_mapping_[bank]);
#pragma message("compiling for 64 bits")
#else
  uint32_t base = reinterpret_cast<uint32_t>(base_mapping_[bank]);
#endif
  if (direction_ == gpio::Direction::kIn) {
    data_  = reinterpret_cast<volatile uint32_t*>(base + gpio::kData);
    setupWait();
  } else {
    setupWait();
    set_   = reinterpret_cast<volatile uint32_t*>(base + gpio::kSet);
    clear_ = reinterpret_cast<volatile uint32_t*>(base + gpio::kClear);
  }
}

////////////////////////////////////////////////////////////////////////////////
void GPIO::setupWait()
{
  int fd;
  char buf[100];

  // setup edge for triggers
  snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/edge", pin_);
  fd = open(buf, O_WRONLY);
  if (fd < 0) {
    log_.ERR("GPIO", "could not open /sys/.../edge for gpio %d", pin_);
    return;
  }
  write(fd, "both", 5);
  close(fd);

  // open /sys/.../value file
  snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", pin_);
  fd = open(buf, O_RDONLY | O_NONBLOCK);
  if (fd < 0) {
    log_.ERR("GPIO", "could not open /sys/.../value for gpio %d", pin_);
    return;
  } else {
    log_.DBG1("GPIO", "gpio %d setup for waiting", pin_);
  }
  fd_ = fd;
}

int8_t GPIO::wait()
{
  pollfd fdset = {};
  fdset.fd = fd_;
  fdset.events = POLLPRI | POLLERR;
  int rc = poll(&fdset, 1, -1);
  if (rc > 0) {
    if (fdset.revents & POLLPRI) {
      log_.DBG1("GPIO", "success Wait on gpio %d", pin_);
      gpio::readHelper(fd_);
      return read();    // assume register access is faster than parsing data from value file
    }

    if (fdset.revents & POLLERR) {
      log_.ERR("GPIO", "an error on wait of gpio %d: %d", pin_, errno);
    }
  }

  log_.ERR("GPIO", "some error on wait of gpio %d", pin_);
  return -1;
}

//-----------------------------------------------------

void GPIO::set()
{
  if (!initialised_) {
    log_.ERR("GPIO", "service has not been initialised");
    return;
  }

  if (!set_) {
    log_.ERR("GPIO", "set register not configured, pin %d", pin_);
    return;
  }

#if GPIOFS
  write(fd_, "1", 2);
#else
  *set_ = pin_mask_;
  log_.DBG1("GPIO", "gpio %d set", pin_);
#endif
}

void GPIO::clear()
{
  if (!initialised_) {
    log_.ERR("GPIO", "service has not been initialised");
    return;
  }

  if (!clear_) {
    log_.ERR("GPIO", "clear register not configured, pin %d", pin_);
    return;
  }
#if GPIOFS
  write(fd_, "0", 2);
#else
  *clear_ = pin_mask_;
  log_.DBG1("GPIO", "gpio %d cleared", pin_);
#endif
}

uint8_t GPIO::read()
{
  if (!initialised_) {
    log_.ERR("GPIO", "service has not been initialised");
    return 0;
  }

  if (!data_) {
    log_.ERR("GPIO", "data register not configured, pin %d", pin_);
    return 0;
  }
#if GPIOFS
  int val = gpio::readHelper(fd_);
  return val;
#else
  // compares data and pin mask whether data is zero or one
  uint8_t val = *data_ & pin_mask_ ? 1 : 0;
  log_.DBG1("GPIO", "gpio %d read %d", pin_, val);
  return val;
  #endif
}

}}}   // namespace hyped::utils::io
