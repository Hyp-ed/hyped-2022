#include "main.hpp"

#include <iostream>

namespace hyped {
namespace navigation {

Main::Main(uint8_t id, Logger &log)
    : Thread(id, log),
      log_(log),
      sys_(System::getSystem()),
      nav_(log, sys_.axis)
{
}

void Main::run()
{
  log_.INFO("NAV", "Axis: %d", sys_.axis);
  log_.INFO("NAV", "Navigation waiting for calibration");

  Data &data               = Data::getInstance();
  bool navigation_complete = false;

  if (!sys_.official_run) nav_.disableKeyenceUsage();
  if (sys_.fake_keyence) nav_.setKeyenceFake();
  if (sys_.enable_nav_write) nav_.logWrite();

  // Setting module status for state machine transition
  data::Navigation nav_data = data.getNavigationData();
  nav_data.module_status    = ModuleStatus::kInit;
  data.setNavigationData(nav_data);

  // wait for calibration state for calibration
  while (sys_.running_ && !navigation_complete) {
    State current_state = data.getStateMachineData().current_state;

    switch (current_state) {
      case State::kIdle:
      case State::kReady:
        break;
      case State::kCalibrating:
        if (nav_.getModuleStatus() == ModuleStatus::kInit) { nav_.calibrateGravity(); }
        break;
      case State::kAccelerating:
        if (!nav_.getHasInit()) {
          nav_.initialiseTimestamps();
          nav_.setHasInit();
        }
        nav_.navigate();
        break;
      case State::kNominalBraking:
      case State::kCruising:
      case State::kEmergencyBraking:
        nav_.navigate();
        break;
      case State::kFailureStopped:
      case State::kFinished:
      case State::kInvalid:
        navigation_complete = true;
        break;
    }
  }
}
}  // namespace navigation
}  // namespace hyped
