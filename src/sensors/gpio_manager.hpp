/*
 * Author:
 * Organisation: HYPED
 * Date: 20/06/19
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

#include "sensors/manager_interface.hpp"

#include "utils/system.hpp"
#include "utils/config.hpp"
#include "utils/io/gpio.hpp"

namespace hyped {

using utils::Logger;
using data::Data;
using utils::io::GPIO;

namespace sensors {

class GpioManager : public GpioManagerInterface  {
 public:
  explicit GpioManager(Logger& log);
  void run() override;

 private:
  utils::System&  sys_;
  data::Data&     data_;

  void clearHP();

  void setHP();

  /**
   * @brief master switch to keep pod on, signal held high at startup
   */
  GPIO* master_;

  /**
   * @brief SSR switches for HP battery packs
   *
   */
  std::vector<GPIO*> hp_ssr_;      // TODO(anyone): to change

  /**
   * @brief print log messages once
   */
  data::State previous_state_;

  /**
   * @brief print log messages once
   */
  data::ModuleStatus previous_battery_status_;
};

}}  // namespace hyped::sensors

#endif  // SENSORS_GPIO_MANAGER_HPP_
