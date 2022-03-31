#include "utils/io/i2c.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>

#if LINUX
#include <linux/i2c-dev.h>
#else
#define I2C_SLAVE 0x0703
#endif

#include "utils/logger.hpp"
#include "utils/system.hpp"

namespace hyped::utils::io {

I2c::I2c(const uint8_t busAddr)
    : log_(utils::Logger("I2C", utils::System::getSystem().config_.log_level)),
      sensor_addr_(0)
{
  char path[13];  // up to "/dev/i2c-255"
  sprintf(path, "/dev/i2c-%d", busAddr);
  fd_ = open(path, O_RDWR, 0);
  if (fd_ < 0) { log_.error("Could not open i2c device"); };
}

I2c::~I2c()
{
  close(fd_);
}

void I2c::setSensorAddress(uint32_t addr)
{
  if (fd_ < 0) {
    log_.error("Could not find i2c device");
    return;
  }

  sensor_addr_ = addr;
  int ret      = ioctl(fd_, I2C_SLAVE, addr);
  if (ret < 0) {
    log_.error("Could not set sensor address");
    return;
  }
}

void I2c::readData(const uint32_t addr, uint8_t *data, const size_t len)
{
  if (fd_ < 0) {
    log_.error("Could not find i2c device");
    return;
  }

  if (sensor_addr_ != addr) { setSensorAddress(addr); }

  const auto ret = read(fd_, data, len);
  if (ret != len) {
    log_.error("Could not read from i2c device");
    return;
  }
}

void I2c::writeData(const uint32_t addr, uint8_t *data, const size_t len)
{
  if (fd_ < 0) {
    log_.error("Could not find i2c device");
    return;
  }
  if (sensor_addr_ != addr) { setSensorAddress(addr); }

  const auto ret = write(fd_, data, len);
  if (ret != len) {
    log_.error("Could not write to i2c device");
    return;
  }
}
}  // namespace hyped::utils::io