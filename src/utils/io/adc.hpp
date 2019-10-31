/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 1/6/19
 * Description: analog read from ADC pins on BBB via file system
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

#ifndef UTILS_IO_ADC_HPP_
#define UTILS_IO_ADC_HPP_

#include <cstdint>
#include <vector>

#include "utils/utils.hpp"
#include "utils/logger.hpp"

namespace hyped {
namespace utils {
// Forward declaration
class Logger;
namespace io {

namespace adc {
}   // namespace adc

class ADC {
 public:
 /**
  * @brief Construct a new ADC object when logger no initialised
  *
  * @param pin
  */
  explicit ADC(uint32_t pin);

  /**
   * @brief Construct a new ADC object with logger for debugging purposes
   *
   * @param pin
   * @param log
   */
  ADC(uint32_t pin, Logger& log);

  /**
   * @brief reads AIN value from file system
   *
   * @return uint16_t return two bytes for [0,4095] range
   */
  uint16_t read();

 private:
  uint32_t      pin_;
  Logger&       log_;
  int           fd_;
};
}}}

#endif  // UTILS_IO_ADC_HPP_
