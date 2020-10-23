
/*
 * Authors: Kornelija Sukyte
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
#include "state_machine/state.hpp"

namespace hyped {

namespace state_machine {

State::State(Logger& log, Main* state_machine)
  : log_(log),
    data_(data::Data::getInstance()),
    state_machine_(state_machine)
{}

void State::checkEmergencyStop()
{
  data::Telemetry telemetry_data = data_.getTelemetryData();
  data::StateMachine sm_data     = data_.getStateMachineData();

  if (telemetry_data.emergency_stop_command) {
    log_.ERR("STM", "STOP command received");
    telemetry_data.emergency_stop_command = false;
    data_.setTelemetryData(telemetry_data);

    sm_data.current_state = data::State::kFinished;
    data_.setStateMachineData(sm_data);

    state_machine_->current_state_ = state_machine_->finished_;
  }
}

// Idling state

void Idling::transitionCheck()
{
  // TODO(Efe): Implement this
}

// Calibrating state

void Calibrating::transitionCheck()
{
  // TODO(Efe): Implement this
}

// Ready state

void Ready::transitionCheck()
{
  data::Telemetry telemetry_data = data_.getTelemetryData();
  data::StateMachine sm_data     = data_.getStateMachineData();

  if (telemetry_data.launch_command) {
    log_.INFO("STM", "launch command received");
    telemetry_data.launch_command = false;
    data_.setTelemetryData(telemetry_data);
    log_.DBG("STM", "launch command cleared");

    sm_data.current_state = data::State::kAccelerating;
    data_.setStateMachineData(sm_data);

    state_machine_->current_state_ = state_machine_->accelerating_;
    log_.DBG("STM", "Transitioned to 'Accelerating'");
  }
}

// Accelerating state

void Accelerating::transitionCheck()
{
  data::Navigation navigation_data = data_.getNavigationData();
  data::Telemetry telemetry_data   = data_.getTelemetryData();
  data::StateMachine sm_data       = data_.getStateMachineData();

  if (navigation_data.displacement +
      navigation_data.braking_distance +
      navigation_data.braking_distance >= telemetry_data.run_length) {
    log_.INFO("STM", "max distance reached");
    log_.INFO("STM", "current distance: %fm, braking distance: %fm",
              navigation_data.displacement, navigation_data.braking_distance);

    sm_data.current_state = data::State::kNominalBraking;
    data_.setStateMachineData(sm_data);

    state_machine_->current_state_ = state_machine_->nominal_braking_;
    log_.DBG("STM", "Transitioned to 'Nominal Braking'");
  }
}

// Braking state

void NominalBraking::transitionCheck()
{
  data::Navigation navigation_data      = data_.getNavigationData();
  data::StateMachine sm_data            = data_.getStateMachineData();

  if (navigation_data.velocity <= 0) {
    log_.INFO("STM", "zero velocity reached");

    sm_data.current_state = data::State::kFinished;
    data_.setStateMachineData(sm_data);

    state_machine_->current_state_ = state_machine_->finished_;
    log_.INFO("STM", "Transitioned to 'Finished'");
  }
}

// Finished state

void Finished::transitionCheck()
{
  utils::System& sys             = utils::System::getSystem();
  data::Telemetry telemetry_data = data_.getTelemetryData();

  if (telemetry_data.reset_command) {
    log_.INFO("STM", "Reset command received");
    log_.INFO("STM", "System is shutting down");
    sys.running_ = false;
  }
}

void FailureBraking::transitionCheck()
{
  // TODO(Franz): Implement this.
}

void FailureStopped::transitionCheck()
{
  // TODO(Yining): Implment this.
}

}  // namespace state_machine
}  // namespace hyped
