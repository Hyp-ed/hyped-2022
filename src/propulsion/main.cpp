#include "main.hpp"

namespace hyped::propulsion {

Main::Main()
    : utils::concurrent::Thread(
      utils::Logger("PROPULSION", utils::System::getSystem().config_.log_level_propulsion)),
      is_running_(true),
      state_processor_()
{
}

bool Main::handleTransition()
{
  if (current_state_ == previous_state_) return false;
  previous_state_ = current_state_;

  log_.info("Entered %s state", data::stateToString(current_state_)->c_str());
  return true;
}

void Main::handleCriticalFailure(data::Data &data, data::Motors &motor_data)
{
  is_running_              = false;
  motor_data.module_status = data::ModuleStatus::kCriticalFailure;
  data.setMotorData(motor_data);
}

void Main::run()
{
  auto &system    = utils::System::getSystem();
  auto &data      = data::Data::getInstance();
  auto motor_data = data.getMotorData();

  // Initialise states
  current_state_  = data.getStateMachineData().current_state;
  previous_state_ = data::State::kInvalid;

  // kInit for state machine transition
  motor_data.module_status = data::ModuleStatus::kInit;
  data.setMotorData(motor_data);
  log_.info("Initialisation complete");

  while (is_running_ && system.isRunning()) {
    // Get the current state of the system from the state machine's data
    data::Motors motor_data     = data.getMotorData();
    data::State current_state_  = data.getStateMachineData().current_state;
    bool encountered_transition = handleTransition();

    switch (current_state_) {
      case data::State::kIdle:
      case data::State::kPreCalibrating:
        break;
      case data::State::kCalibrating:
        if (state_processor_.isInitialised()) {
          if (motor_data.module_status != data::ModuleStatus::kReady) {
            motor_data.module_status = data::ModuleStatus::kReady;
            data.setMotorData(motor_data);
          }
        } else {
          state_processor_.initialiseMotors();
          if (!state_processor_.isInitialised()) { handleCriticalFailure(data, motor_data); }
        }
        break;
      case data::State::kPreReady:
        break;
      case data::State::kReady:
        if (encountered_transition) { state_processor_.sendOperationalCommand(); }
        break;
      case data::State::kAccelerating:
        if (state_processor_.isOverLimits()) {
          handleCriticalFailure(data, motor_data);
          break;
        }
        state_processor_.accelerate();
        break;
      case data::State::kCruising:
      case data::State::kPreBraking:
      case data::State::kNominalBraking:
      case data::State::kFailurePreBraking:
      case data::State::kFailureBraking:
        state_processor_.quickStopAll();
        break;
      case data::State::kFailureStopped:
      case data::State::kFinished:
      case data::State::kInvalid:
        handleCriticalFailure(data, motor_data);
        break;
    }
  }

  log_.info("Thread shutting down");
}
}  // namespace hyped::propulsion
