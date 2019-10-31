/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 1/6/19
 * Description: scales thermistor voltage reading from ADC pin and creates TemperatureData instance
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

#ifndef SENSORS_TEMPERATURE_HPP_
#define SENSORS_TEMPERATURE_HPP_

#include <cstdint>

#include "data/data.hpp"
#include "utils/logger.hpp"
#include "sensors/interface.hpp"

namespace hyped {

using utils::Logger;

namespace sensors {

class Temperature: public TemperatureInterface {
 public:
  /**
   * @brief Construct a new Temperature object
   *
   * @param log from main thread, for debugging purposes
   * @param pin for specific ADC pin
   */
  Temperature(utils::Logger& log, int pin);
  ~Temperature() {}

  /**
   * @brief
   *
   * @return int to set to data struct in sensors main
   */
  int getData() override;

  /**
   * @brief one interation of checking sensors
   */
  void run() override;

 private:
  /**
   * @brief scale raw digital data to output in degrees C
   *
   * @param raw_value input voltage
   * @return int representation of temperature
   */
  int scaleData(uint16_t raw_value);

  /**
   * @brief ADC pin
   */
  int pin_;
  utils::Logger& log_;

  /**
   * @brief int from data structs
   */
  data::TemperatureData temp_;
};

}}

#endif  // SENSORS_TEMPERATURE_HPP_
