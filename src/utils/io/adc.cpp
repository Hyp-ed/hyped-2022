#include "adc.hpp"

#include <fcntl.h>   // define O_WONLY and O_RDONLY
#include <unistd.h>  // close()

#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped {
namespace utils {
namespace io {
namespace adc {

uint16_t readHelper(int fd)
{
  char buf[4];                 // buffer size 4 for fs value
  lseek(fd, 0, SEEK_SET);      // reset file pointer
  read(fd, buf, sizeof(buf));  // actually consume new data, changes value in buffer
  return std::atoi(buf);
}

}  // namespace adc

Adc::Adc(uint32_t pin) : Adc(pin, System::getLogger())
{ /* EMPTY, delegate to the other constructor */
}

Adc::Adc(uint32_t pin, Logger &log) : pin_(pin), log_(log), fd_(0)
{
}

uint16_t Adc::read()
{
  char buf[100];
  snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio:device0/in_voltage%i_raw", pin_);
  fd_ = open(buf, O_RDONLY);
  if (fd_ < 0) { log_.error("problem reading pin %d raw voltage", pin_); }
  log_.debug("fd: %d", fd_);
  uint16_t val = adc::readHelper(fd_);
  log_.debug("val: %d", val);
  close(fd_);
  return val;
}

}  // namespace io
}  // namespace utils
}  // namespace hyped
