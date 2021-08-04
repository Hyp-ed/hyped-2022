
/*
 * Authors: Kornelija Sukyte, Franz Miltz
 * Organisation: HYPED
 * Date:
 * Description: Here we declare the general state and the layout of all the specific states. We do
 * not specify actual behaviour.
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
#pragma once

#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>
#include <utils/timer.hpp>

#include "main.hpp"
#include "transitions.hpp"

namespace hyped {

using data::ModuleStatus;
using utils::Logger;

namespace state_machine {

class Main;  // Forward declaration

class State {
 public:
  State();
  static State *getInstance();

  virtual void enter(Logger &log) = 0;
  virtual void exit(Logger &log)  = 0;

  virtual State *checkTransition(Logger &log) = 0;

  data::Data &data_;

 protected:
  data::EmergencyBrakes embrakes_data_;
  data::Navigation nav_data_;
  data::Batteries batteries_data_;
  data::Telemetry telemetry_data_;
  data::Sensors sensors_data_;
  data::Motors motors_data_;
  void updateModuleData();
};

class Messages;

/*
 * @brief   Generates a specific state S following the pattern of State.
 */
#define MAKE_STATE(S)                                                                              \
  class S : public State {                                                                         \
   public:                                                                                         \
    S() {}                                                                                         \
    static S *getInstance() { return &S::instance_; }                                              \
                                                                                                   \
    State *checkTransition(Logger &log);                                                           \
    /* @brief   Prints log message and sets appropriate public enum value.*/                       \
    void enter(Logger &log)                                                                        \
    {                                                                                              \
      log.INFO("STM", "entering %s state", S::string_representation_);                             \
      data::StateMachine sm_data = data_.getStateMachineData();                                    \
      sm_data.current_state      = S::enum_value_;                                                 \
      data_.setStateMachineData(sm_data);                                                          \
    }                                                                                              \
    void exit(Logger &log) { log.INFO("STM", "exiting %s state", S::string_representation_); }     \
                                                                                                   \
   private:                                                                                        \
    static S instance_;                                                                            \
    /* # converts an argument to a string literal*/                                                \
    static char string_representation_[];                                                          \
    static data::State enum_value_;                                                                \
  };

/*
 * Generating structs for all the states
 */

MAKE_STATE(Idle)            // State on startup
MAKE_STATE(Calibrating)     // Calibrating starts after user input is given
MAKE_STATE(Ready)           // After calibration has finished
MAKE_STATE(Accelerating)    // First phase of the run
MAKE_STATE(Cruising)        // Intermediate phase to not exceed maximum velocity
MAKE_STATE(NominalBraking)  // Second phase of the run
MAKE_STATE(Finished)        // State after the run
MAKE_STATE(FailureBraking)  // Entered upon failure during the run
MAKE_STATE(FailureStopped)  // Entered upon failure before the run or after
                            // FailureBraking

#undef MAKE_STATE

// We need to implement Off separately because it works a bit differently
class Off : public State {
 public:
  Off() {}
  static Off *getInstance() { return &Off::instance_; }

  State *checkTransition(Logger &log);

  void enter(Logger &log)
  {
    log.INFO("STM", "shutting down");
    utils::System &sys = utils::System::getSystem();
    sys.running_       = false;
  }

  void exit(Logger &log)
  {  // We never exit this state
  }

 private:
  static Off instance_;
};

}  // namespace state_machine
}  // namespace hyped
