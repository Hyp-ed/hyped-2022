/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description:
 *    Manages basic GPIO output for state machine and module responses for easy additions and edits
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

#ifndef SENSORS_GPIO_MANAGER_HPP_
#define SENSORS_GPIO_MANAGER_HPP_

#include <cstdint>
#include <vector>

#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

#include "utils/system.hpp"
#include "utils/config.hpp"
#include "utils/io/gpio.hpp"

namespace hyped {

using utils::Logger;
using data::Data;
using utils::concurrent::Thread;
using utils::io::GPIO;

namespace sensors {
/**
 * Please see wiki for high-level description of this shared class for our system.
 * For new hardware devices, you must add: (vector of) GPIO*, previous_<module>_status_, and
 * appropriate hardware actuation functions. Use switch statements in run() to check status/state
 * and call appropriate functions. You should not need to other files since switch statements should
 * check data directly from data.hpp.g
 */

class GpioManager : public Thread  {
 public:
  explicit GpioManager(Logger& log);
  void run() override;

 private:
  void clearHP();

  void setHP();

  utils::System&  sys_;
  data::Data&     data_;
  /**
   * @brief master switch to keep pod on, signal held high at startup
   */
  GPIO* master_;

  /**
   * @brief SSR switches for HP battery packs
   *
   */
  std::vector<GPIO*> hp_ssr_;

  /**
   * @brief stores the previous state when switch statement checks state machine
   *        conditional statement prevents repetitive actuation
   */
  data::State previous_state_;

  /**
   * @brief stores the previous state when switch statement checks state machine
   *        conditional statement prevents repetitive actuation
   */
  data::ModuleStatus previous_battery_status_;
};

}}  // namespace hyped::sensors

#endif  // SENSORS_GPIO_MANAGER_HPP_
