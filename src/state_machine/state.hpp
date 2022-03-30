#pragma once

#include "transitions.hpp"

#include <string>

#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>
#include <utils/timer.hpp>

namespace hyped::state_machine {

class State {
 public:
  State();
  static State *getInstance();

  virtual void enter(utils::Logger &log) = 0;
  virtual void exit(utils::Logger &log)  = 0;

  virtual State *checkTransition(utils::Logger &log) = 0;

  data::Data &data_;

 protected:
  data::Brakes brakes_data_;
  data::Navigation nav_data_;
  data::FullBatteryData batteries_data_;
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
    static S *getInstance() { return &S::instance_; }                                              \
                                                                                                   \
    State *checkTransition(utils::Logger &log);                                                    \
    /* @brief   Prints log message and sets appropriate public enum value.*/                       \
    void enter(utils::Logger &log)                                                                 \
    {                                                                                              \
      log.info("entering %s state", S::string_representation_);                                    \
      data::StateMachine sm_data = data_.getStateMachineData();                                    \
      sm_data.current_state      = S::enum_value_;                                                 \
      data_.setStateMachineData(sm_data);                                                          \
    }                                                                                              \
    void exit(utils::Logger &log) { log.info("exiting %s state", S::string_representation_); }     \
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
MAKE_STATE(PreReady)           // After calibration has finished
MAKE_STATE(Ready)              // Entered after high power is on
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

  State *checkTransition(utils::Logger &log);

  void enter(utils::Logger &log)
  {
    log.info("shutting down");
    utils::System &sys = utils::System::getSystem();
    sys.stop();
  }

  void exit(utils::Logger &)
  {  // We never exit this state
  }

 private:
  static Off instance_;
};

}  // namespace hyped::state_machine
