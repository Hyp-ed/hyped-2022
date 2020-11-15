
/*
 * Authors: Kornelija Sukyte, Franz Miltz
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
#include "state_machine/transitions.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/timer.hpp"

namespace hyped {

using data::ModuleStatus;
using utils::Logger;

namespace state_machine {

class Main;  // Forward declaration

class State {
 public:
  State();
  static State *getInstance();
  static void initialise();

  virtual void enter(Logger &log) = 0;
  virtual void exit(Logger &log)  = 0;

  virtual State *checkTransition(Logger &log) = 0;

  data::Data &data_;

 protected:
  static State *instance_;
  data::EmergencyBrakes embrakes_data_;
  data::Navigation nav_data_;
  data::Batteries batteries_data_;
  data::Telemetry telemetry_data_;
  data::Sensors sensors_data_;
  data::Motors motors_data_;
  void updateModuleData();
};

/*
 * @brief   Generates a specific state S following the pattern of State.
 */
#define MAKE_STATE(S)                                                                              \
  class S : public State {                                                                         \
   public:                                                                                         \
    S() {}                                                                                         \
    static S *instance_;                                                                           \
    static S *getInstance() { return S::instance_; }                                               \
    static void initialise() { S::instance_ = new S(); }                                           \
                                                                                                   \
    State *checkTransition(Logger &log);                                                           \
    void enter(Logger &log);                                                                       \
    void exit(Logger &log);                                                                        \
  };

/*
 * Generating structs for all the states
 */

MAKE_STATE(Idling)          // State on startup
MAKE_STATE(Calibrating)     // Calibrating starts after user input is given
MAKE_STATE(Ready)           // After calibration has finished
MAKE_STATE(Accelerating)    // First phase of the run
MAKE_STATE(NominalBraking)  // Second phase of the run
MAKE_STATE(Finished)        // State after the run
MAKE_STATE(FailureBraking)  // Entered upon failure during the run
MAKE_STATE(FailureStopped)  // Entered upon failure before the run or after
                            // FailureBraking
MAKE_STATE(Off)             // Only exists for the enter() method

}  // namespace state_machine
}  // namespace hyped

#endif  // STATE_MACHINE_STATE_HPP_
