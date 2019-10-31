/*
 * Author: Gregory Dayao and Jack Horsburgh
 * Organisation: HYPED
 * Date: 6/06/19
 * Description: Main class for fake.
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

#ifndef SENSORS_FAKE_TEMPERATURE_HPP_
#define SENSORS_FAKE_TEMPERATURE_HPP_

#include <string>

#include "utils/logger.hpp"
#include "sensors/interface.hpp"

namespace hyped {

using utils::Logger;
using data::Data;

namespace sensors {

class FakeTemperature: public TemperatureInterface {
 public:
  /**
   * @brief Construct a new Fake Temperature object
   *
   * @param log
   * @param is_fail
   */
  FakeTemperature(Logger& log, bool is_fail);

  /**
   * @brief returns int representation
   *
   * @return int temperature degrees C
   */
  int getData() override;

  /**
   * @brief waits for acceleration, generate random time for failure
   */
  void run() override;

 private:
  Data& data_;
  utils::Logger& log_;

  /**
   * @brief dependent on is_fail_, set to fail value
   */
  void checkFailure();

  // values degrees C
  int failure_;
  int success_;
  data::TemperatureData temp_;

  bool is_fail_;

  uint64_t acc_start_time_;
  bool acc_started_;
  uint64_t failure_time_;
  bool failure_happened_;
};
}}    // namespace hyped::sensors

#endif  // SENSORS_FAKE_TEMPERATURE_HPP_
