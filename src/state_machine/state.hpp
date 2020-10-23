
/*
 * Authors: Kornelija Sukyte
 * Organisation: HYPED
 * Date:
 * Description:
 *
 *    Copyright 2020 HYPED
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

#ifndef STATE_MACHINE_STATE_HPP_
#define STATE_MACHINE_STATE_HPP_

#include "data/data.hpp"
#include "state_machine/main.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/timer.hpp"

namespace hyped {

using utils::Logger;
using data::ModuleStatus;

namespace state_machine {

class Main;  // Forward declaration

class State {
 public:
  State(Logger& log, Main* state_machine);

  void checkEmergencyStop();

  virtual void transitionCheck() = 0;

  Logger&               log_;
  data::Data&           data_;

  data::Telemetry       telemetry_data_;
  data::Navigation      nav_data_;
  data::StateMachine    sm_data_;
  data::Motors          motor_data_;
  data::Sensors         sensors_data_;
  data::EmergencyBrakes brakes_data_;

 protected:
  Main* state_machine_;
};

class Ready : public State {
 public:
  Ready(Logger& log, Main* state_machine) : State(log, state_machine) {}

  /*
   * @brief   Checks for launch command
   */
  void transitionCheck();
};

class Accelerating : public State {
 public:
  Accelerating(Logger& log, Main* state_machine) : State(log, state_machine) {}

  /*
   * @brief   Checks if max distance reached
   */
  void transitionCheck();
};

class Braking : public State {
 public:
  Braking(Logger& log, Main* state_machine) : State(log, state_machine) {}
  void transitionCheck();
};

class Finished : public State {
 public:
  Finished(Logger& log, Main* state_machine) : State(log, state_machine) {}

  /*
   * @brief   Checks if command to reset was sent
   */
  void transitionCheck();
};

}  // namespace state_machine
}  // namespace hyped

#endif  // STATE_MACHINE_STATE_HPP_
