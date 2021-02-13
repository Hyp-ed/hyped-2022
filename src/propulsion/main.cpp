/*
 * Author: Gregor Konzett, Franz Miltz
 * Organisation: HYPED
 * Date: 13.02.2021
 * Description: Main entrypoint to motor control module
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "propulsion/main.hpp"

namespace hyped {

namespace motor_control {
Main::Main(uint8_t id, Logger &log)
    : Thread(id, log),
      is_running_(true),
      log_(log),
      state_processor_(new StateProcessor(Motors::kNumMotors, log))
{
}

bool Main::handleTransition()
{
  if (current_state_ == previous_state_) return false;

  log_.INFO("Motor", "Entered %s state", data::states[current_state_]);
  return true;
}

void Main::handleCriticalFailure(Data &data, Motors &motor_data)
{
  is_running_              = false;
  motor_data.module_status = ModuleStatus::kCriticalFailure;
  data.setMotorData(motor_data);
}

void Main::run()
{
  utils::System &sys      = utils::System::getSystem();
  data::Data &data        = data::Data::getInstance();
  data::Motors motor_data = data.getMotorData();

  // Initialise states
  current_state_  = data.getStateMachineData().current_state;
  previous_state_ = State::kInvalid;

  // kInit for SM transition
  motor_data.module_status = ModuleStatus::kInit;
  data.setMotorData(motor_data);
  log_.INFO("Motor", "Initialisation complete");

  while (is_running_ && sys.running_) {
    // Get the current state of the system from the state machine's data
    motor_data                  = data.getMotorData();
    current_state_              = data.getStateMachineData().current_state;
    bool encountered_transition = handleTransition();

    switch (current_state_) {
      case State::kIdle:
        break;
      case State::kCalibrating:
        if (state_processor_->isInitialized()) {
          if (motor_data.module_status != ModuleStatus::kReady) {
            motor_data.module_status = ModuleStatus::kReady;
            data.setMotorData(motor_data);
          }
        } else {
          state_processor_->initMotors();
          if (state_processor_->isCriticalFailure()) { handleCriticalFailure(data, motor_data); }
        }
        break;
      case State::kReady:
        if (encountered_transition) { state_processor_->sendOperationalCommand(); }
        break;
      case State::kAccelerating:
        state_processor_->accelerate();
        break;
      case State::kCruising:
        // TODO(Akshath29): Implement cruising behaviour
        break;
      case State::kNominalBraking:
      case State::kEmergencyBraking:
        state_processor_->quickStopAll();
        break;
      default:
        handleCriticalFailure(data, motor_data);
        break;
    }
  }

  log_.INFO("Motor", "Thread shutting down");
}
}  // namespace motor_control
}  // namespace hyped
