#include "main.hpp"

#include <iostream>

namespace hyped {
namespace navigation {

Main::Main(uint8_t id, utils::Logger &log)
    : utils::concurrent::Thread(id, log),
      log_(log),
      sys_(utils::System::getSystem()),
      nav_(log, sys_.axis)
{
}

void Main::run()
{
  log_.INFO("NAV", "Axis: %d", sys_.axis);
  log_.INFO("NAV", "Navigation waiting for calibration");

  data::Data &data         = data::Data::getInstance();
  bool navigation_complete = false;

  if (!sys_.official_run) nav_.disableKeyenceUsage();
  if (sys_.fake_keyence) nav_.setKeyenceFake();
  if (sys_.enable_nav_write) nav_.logWrite();

  // Setting module status for state machine transition
  data::Navigation nav_data = data.getNavigationData();
  nav_data.module_status    = data::ModuleStatus::kInit;
  data.setNavigationData(nav_data);

  // wait for calibration state for calibration
  while (sys_.running_ && !navigation_complete) {
    data::State current_state = data.getStateMachineData().current_state;

    switch (current_state) {
      case data::State::kIdle:
      case data::State::kReady:
        break;
      case data::State::kCalibrating:
        if (nav_.getModuleStatus() == data::ModuleStatus::kInit) { nav_.calibrateGravity(); }
        break;
      case data::State::kAccelerating:
        if (!nav_.getHasInit()) {
          nav_.initialiseTimestamps();
          nav_.setHasInit();
        }
        nav_.navigate();
        break;
      case data::State::kNominalBraking:
      case data::State::kCruising:
      case data::State::kEmergencyBraking:
        nav_.navigate();
        break;
      case data::State::kFailureStopped:
      case data::State::kFinished:
      case data::State::kInvalid:
        navigation_complete = true;
        break;
    }
  }
}
}  // namespace navigation
}  // namespace hyped
