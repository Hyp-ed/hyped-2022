/*
 * Author: Gregory Dayao and Jack Horsburgh
 * Organisation: HYPED
 * Date: 20/06/18
 * Description:
 * BMS manager for getting battery data and pushes to data struct.
 * Checks whether batteries are in range and enters emergency state if fails.
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

#ifndef SENSORS_BMS_MANAGER_HPP_
#define SENSORS_BMS_MANAGER_HPP_

#include <cstdint>

#include "sensors/manager_interface.hpp"

#include "sensors/interface.hpp"
#include "utils/system.hpp"
#include "utils/io/gpio.hpp"

namespace hyped {

using utils::Logger;
using hyped::data::BatteryData;
using utils::io::GPIO;

namespace sensors {

class BmsManager: public ManagerInterface  {
 public:
  explicit BmsManager(Logger& log);
  void run()                override;

 private:
  BMSInterface*   bms_[data::Batteries::kNumLPBatteries+data::Batteries::kNumHPBatteries];
  utils::System&  sys_;

  /**
   * @brief for hp_master_, hp_ssr_, and prop_cool_
   */
  void clearHP();

  /**
   * @brief for hp_master_, hp_ssr_, and prop_cool_
   */
  void setHP();

  /**
   * @brief check IMD and set GPIOs accordingly
   */
  bool checkIMD();

  /**
   * @brief needs to be references because run() passes directly to data struct
   */
  data::Data&     data_;

  /*
   * @brief SSR that controls objects in hp_ssr_ array
   */
  GPIO* hp_master_;

  /**
   * @brief HPSSR held high in nominal states, cleared when module failure or pod emergency state
   *        Batteries module status forces kEmergencyBraking, which actuates embrakes
   */
  GPIO* hp_ssr_[data::Batteries::kNumHPBatteries];

  /**
   * @brief embrakes_ssr_ held high in nominal states, cleared in emergency state
   */
  GPIO* embrakes_ssr_;

  /**
   * @brief for IMD boolean in BatteryData
   */
  GPIO* imd_out_;

  /**
   * @brief holds LP BatteryData, HP BatteryData, and module_status
   */
  data::Batteries batteries_;

  /**
   * @brief print log messages once
   */
  data::State previous_state_;

  /**
   * @brief print log messages once
   */
  data::ModuleStatus previous_status_;

  /**
   * @brief do not check ranges for first 5 seconds
   */
  uint64_t start_time_;
  uint64_t check_time_ = 5000000;

  /**
   * @brief checks voltage, current, temperature, and charge
   */
  bool batteriesInRange();
};

}}  // namespace hyped::sensors

#endif  // SENSORS_BMS_MANAGER_HPP_
