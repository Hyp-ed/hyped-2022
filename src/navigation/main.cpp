#include "main.hpp"

#include <iostream>

namespace hyped::navigation {

Main::Main()
    : utils::concurrent::Thread(
      utils::Logger("NAVIGATION", utils::System::getSystem().config_.log_level_navigation)),
      sys_(utils::System::getSystem()),
      nav_(sys_.config_.axis)
{
}

void Main::run()
{
  log_.info("Axis: %d", sys_.config_.axis);
  log_.info("Navigation waiting for calibration");

  auto &data               = data::Data::getInstance();
  bool navigation_complete = false;

  // Setting module status for state machine transition
  data::Navigation nav_data = data.getNavigationData();
  nav_data.module_status    = data::ModuleStatus::kInit;
  data.setNavigationData(nav_data);

  // wait for calibration state for calibration
  while (sys_.isRunning() && !navigation_complete) {
    auto current_state = data.getStateMachineData().current_state;

    switch (current_state) {
      case data::State::kIdle:
      case data::State::kReady:
      case data::State::kPreCalibrating:
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
}  // namespace hyped::navigation
