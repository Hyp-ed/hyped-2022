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

Adc::Adc(const uint32_t pin) : log_("ADC", utils::System::getSystem().config_.log_level), pin_(pin)
{
}
uint16_t Adc::read()
{
  char buf[100];
  snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio:device0/in_voltage%i_raw", pin_);
  file_ = open(buf, O_RDONLY);
  if (file_ < 0) { log_.error("problem reading pin %d raw voltage", pin_); }
  log_.debug("fd: %d", file_);
  uint16_t val = adc::readHelper(file_);
  log_.debug("val: %d", val);
  close(file_);
  return val;
}

}  // namespace io
}  // namespace utils
}  // namespace hyped
