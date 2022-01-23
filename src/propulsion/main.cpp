#include "main.hpp"

namespace hyped::propulsion {

Main::Main(const uint8_t id, utils::Logger &log)
    : Thread(id, log),
      is_running_(true),
      log_(log),
      state_processor_(log)
{
}

bool Main::handleTransition()
{
  if (current_state_ == previous_state_) return false;
  previous_state_ = current_state_;

  log_.INFO("Motor", "Entered %s state", data::states[current_state_]);
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
  const auto &system = utils::System::getSystem();
  auto &data         = data::Data::getInstance();
  auto motor_data    = data.getMotorData();

  // Initialise states
  current_state_  = data.getStateMachineData().current_state;
  previous_state_ = data::State::kInvalid;

  // kInit for state machine transition
  motor_data.module_status = data::ModuleStatus::kInit;
  data.setMotorData(motor_data);
  log_.INFO("Motor", "Initialisation complete");

  while (is_running_ && system.running_) {
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
      case data::State::kReady:
        if (encountered_transition) { state_processor_.sendOperationalCommand(); }
        break;
      case data::State::kAccelerating:
        if (state_processor_.isOverlimits()) {
          handleCriticalFailure(data, motor_data);
          break;
        }
        state_processor_.accelerate();
        break;
      case data::State::kCruising:
      case data::State::kNominalBraking:
      case data::State::kEmergencyBraking:
        state_processor_.quickStopAll();
        break;
      case data::State::kFailureStopped:
      case data::State::kFinished:
      case data::State::kInvalid:
        handleCriticalFailure(data, motor_data);
        break;
    }
  }

  log_.INFO("Motor", "Thread shutting down");
}
}  // namespace hyped::propulsion
