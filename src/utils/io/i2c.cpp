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

I2C::I2C()
    : log_(utils::Logger("I2C", utils::System::getSystem().config_.log_level)),
      fd_(0),
      sensor_addr_(0)
{
  fd_ = open("/dev/i2c-2", O_RDWR, 0);
  if (fd_ < 0) { log_.error("Could not open i2c device"); };
}

I2C::~I2C()
{
  close(fd_);
}

void I2C::setSensorAddress(uint32_t addr)
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

void I2C::readData(const uint32_t addr, uint8_t *data, const uint8_t len)
{
  if (fd_ < 0) {
    log_.error("Could not find i2c device");
    return;
  }

  if (sensor_addr_ != addr) setSensorAddress(addr);

  int ret = read(fd_, data, len);
  if (ret != len) {
    log_.error("Could not read from i2c device");
    return;
  }
}

void I2C::writeData(const uint32_t addr, uint8_t *data, const uint8_t len)
{
  if (fd_ < 0) {
    log_.error("Could not find i2c device");
    return;
  }
  if (sensor_addr_ != addr) setSensorAddress(addr);

  int ret = write(fd_, data, len);
  if (ret != len) {
    log_.error("Could not write to i2c device");
    return;
  }
}
}  // namespace hyped::utils::io