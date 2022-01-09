#pragma once

#include "main.hpp"
#include "transitions.hpp"

#include <string>

#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>
#include <utils/timer.hpp>

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
  data::EmergencyBrakes brakes_data_;
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

MAKE_STATE(Idle)               // State on startup
MAKE_STATE(PreCalibrating)     // Sub-state between Idle and Calibrating
MAKE_STATE(Calibrating)        // Calibrating starts after user input is given
MAKE_STATE(Ready)              // After calibration has finished
MAKE_STATE(Accelerating)       // First phase of the run
MAKE_STATE(Cruising)           // Intermediate phase to not exceed maximum velocity
MAKE_STATE(PreBraking)         // Sub-state between Accelerating/Cruising and Nominal Braking
MAKE_STATE(NominalBraking)     // Second phase of the run
MAKE_STATE(Finished)           // State after the run
MAKE_STATE(FailurePreBraking)  // Sub-state entered before FailureBraking
MAKE_STATE(FailureBraking)     // Entered upon failure during the run
MAKE_STATE(FailureStopped)     // Entered upon failure before the run or after
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
