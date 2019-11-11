/*
* Author: Kornelija Sukyte
* Organisation: HYPED
* Date:
* Description:
*
*    Copyright 2019 HYPED
*    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
*    except in compliance with the License. You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
*    Unless required by applicable law or agreed to in writing, software distributed under
*    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
*    either express or implied. See the License for the specific language governing permissions and
*    limitations under the License.
*/

#ifndef EMBRAKES_STEPPER_HPP_
#define EMBRAKES_STEPPER_HPP_

#include "utils/timer.hpp"
#include "utils/logger.hpp"
#include "utils/io/gpio.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::Logger;
using utils::io::GPIO;
using data::ModuleStatus;

namespace embrakes {

class Stepper {
 public:
  /**
   * @brief Construct a new Stepper object
   * @param log, node id
   */
  Stepper(uint8_t enable_pin, uint8_t button_pin, Logger& log, uint8_t id);

  /**
   * @brief {checks if brake's button is pressed, notes change in the data struct}
   */
  void checkHome();

  /**
   * @brief sends retract message
   */
  void sendRetract();

  /**
   * @brief sends clamp message
   */
  void sendClamp();

  /**
   * @brief checks for brake failure during acceleration
   */
  void checkAccFailure();

  void checkBrakingFailure();

  bool checkClamped();

 private:
  utils::Logger&        log_;
  data::Data&           data_;
  data::EmergencyBrakes em_brakes_data_;
  GPIO                  command_pin_;
  GPIO                  button_;
  uint8_t               brake_id_;
  uint8_t               is_clamped_;
  uint64_t              timer;
};

}}  // namespace hyped::embrakes

#endif  // EMBRAKES_STEPPER_HPP_
