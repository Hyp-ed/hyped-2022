/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description:
 * Main initialises and manages sensor drivers. Main is not responsible for initialisation
 * of supporting io drivers (spi, can, adc). This should be done by the sensor
 * drivers themselves.
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
#pragma once

#include <cstdint>
#include <utils/system.hpp>

#include "bms_manager.hpp"
#include "imu_manager.hpp"
#include "interface.hpp"

namespace hyped {

namespace sensors {

/**
 * @brief Initialise sensors, data instances to be pulled in managers
 *        gpio threads and adc checks declared in main
 */
class Main : public Thread {
 public:
  Main(uint8_t id, utils::Logger &log);
  void run() override;  // from thread

 private:
  /**
   * @brief as long as at least one keyence value is updated
   *
   * @return true
   * @return false
   */
  bool keyencesUpdated();

  /**
   * @brief checks range of pod temperature
   *
   * @return true if status is valid
   * @return false if kCriticalFailure
   */
  bool temperatureInRange();

  /**
   * @brief used to check the temperature infrequently in main loop,
   *        unnecessary to constantly check temperature;
   */
  void checkTemperature();

  data::Data &data_;
  utils::System &sys_;
  utils::Logger &log_;

  // master data structures
  data::Sensors sensors_;
  data::Batteries batteries_;
  data::StripeCounter stripe_counter_;

  uint8_t pins_[data::Sensors::kNumKeyence];
  GpioInterface *keyences_[data::Sensors::kNumKeyence];  // 0 L and 1 R
  ImuManager *imu_manager_;
  BmsManager *battery_manager_;
  TemperatureInterface *temperature_;
  bool log_error_ = false;

  /**
   * @brief update this from GpioCounter::getStripeCounter();
   */
  array<data::StripeCounter, data::Sensors::kNumKeyence> keyence_stripe_counter_arr_;

  /**
   * @brief use this to compare with keyence_stripe_counter_arr_
   *        update when keyenceUpdated() == true
   */
  array<data::StripeCounter, data::Sensors::kNumKeyence> prev_keyence_stripe_count_arr_;
};

}  // namespace sensors
}  // namespace hyped
