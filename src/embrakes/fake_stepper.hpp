/*
 * Author: Atte Niemi
 * Organisation: HYPED
 * Date:
 * Description:
 *
 *    Copyright 2020 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain a
 * copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#ifndef EMBRAKES_FAKE_STEPPER_HPP_
#define EMBRAKES_FAKE_STEPPER_HPP_

#include "data/data.hpp"
#include "embrakes/interface.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped {

using data::ModuleStatus;
using utils::Logger;
using utils::concurrent::Thread;

namespace embrakes {

class FakeStepper : public StepperInterface {
 public:
  /**
   * @brief Construct a new Stepper object
   * @param log, node id
   */
  FakeStepper(Logger& log, uint8_t id);

  /**
   * @brief {checks if brake's button is pressed, notes change in the data
   * struct}
   */
  void checkHome() override;

  /**
   * @brief sends retract message
   */
  void sendRetract() override;

  /**
   * @brief sends clamp message
   */
  void sendClamp() override;

  /**
   * @brief checks for brake failure during acceleration
   */
  void checkAccFailure() override;

  void checkBrakingFailure() override;

  bool checkClamped() override;

 private:
  bool fake_button_;
  utils::Logger& log_;
  data::Data& data_;
  data::EmergencyBrakes em_brakes_data_;
  uint8_t brake_id_;
  uint8_t is_clamped_;
};

}  // namespace embrakes
}  // namespace hyped

#endif  // EMBRAKES_FAKE_STEPPER_HPP_
