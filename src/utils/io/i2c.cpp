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

I2c::I2c(const uint8_t bus_address)
    : log_(utils::Logger("I2C", utils::System::getSystem().config_.log_level)),
      sensor_address_(0)
{
  char path[13];  // up to "/dev/i2c-255"
  sprintf(path, "/dev/i2c-%d", bus_address);
  fd_ = open(path, O_RDWR, 0);
  if (fd_ < 0) { log_.error("Could not open i2c device"); };
}

I2c::~I2c()
{
  close(fd_);
}

void I2c::setSensorAddress(uint32_t address)
{
  if (fd_ < 0) {
    log_.error("Could not find i2c device");
    return;
  }

  sensor_address_ = address;
  const int ret   = ioctl(fd_, I2C_SLAVE, address);
  if (ret < 0) {
    log_.error("Could not set sensor address");
    return;
  }
}

int I2c::readData(const uint32_t address, uint8_t *data, const size_t len)
{
  if (fd_ < 0) {
    log_.error("Could not find i2c device");
    return -1;
  }

  if (sensor_address_ != address) { setSensorAddress(address); }

  const auto ret = read(fd_, data, len);
  if (ret != static_cast<int>(len)) {
    log_.error("Could not read from i2c device");
    return -1;
  }
  return ret;
}

int I2c::writeData(const uint32_t address, uint8_t *data, const size_t len)
{
  if (fd_ < 0) {
    log_.error("Could not find i2c device");
    return -1;
  }
  if (sensor_address_ != address) { setSensorAddress(address); }

  const auto ret = write(fd_, data, len);
  if (ret != static_cast<int>(len)) {
    log_.error("Could not write to i2c device");
    return -1;
  }
  return ret;
}
}  // namespace hyped::utils::io