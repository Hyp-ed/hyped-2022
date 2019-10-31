/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 1/6/19
 * Description:
 *
 *    Copyright 2019 HYPED
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

#include "utils/io/adc.hpp"

#include <fcntl.h>      // define O_WONLY and O_RDONLY
#include <unistd.h>     // close()

#include "utils/logger.hpp"
#include "utils/system.hpp"

namespace hyped {
namespace utils {
namespace io {
namespace adc {

uint16_t readHelper(int fd)
{
  char buf[4];                                 // buffer size 4 for fs value
  lseek(fd, 0, SEEK_SET);                      // reset file pointer
  read(fd, buf, sizeof(buf));                  // actually consume new data, changes value in buffer
  return std::atoi(buf);
}

}   // namespace adc

ADC::ADC(uint32_t pin)
    : ADC(pin, System::getLogger())
{ /* EMPTY, delegate to the other constructor */ }

ADC::ADC(uint32_t pin, Logger& log)
    : pin_(pin),
      log_(log),
      fd_(0)
{}

uint16_t ADC::read()
{
  char buf[100];
  snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio:device0/in_voltage%i_raw", pin_);
  fd_ = open(buf, O_RDONLY);
  if (fd_ < 0) {
    log_.ERR("ADC", "problem reading pin %d raw voltage", pin_);
  }
  log_.DBG1("ADC", "fd: %d", fd_);
  uint16_t val = adc::readHelper(fd_);
  log_.DBG1("ADC", "val: %d", val);
  close(fd_);
  return val;
}


}}}   // namespace hyped::utils::io
