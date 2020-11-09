
/*
 * Authors: Kornelija Sukyte, Franz Miltz, Efe Ozbatur, Yining Wang
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

State::State(Logger &log, Main *state_machine)
    : log_(log),
      data_(data::Data::getInstance()),
      state_machine_(state_machine)
{
}

void State::checkEmergencyStop()
{
  data::EmergencyBrakes embrakes_data = data_.getEmergencyBrakesData();
  data::StateMachine sm_data          = data_.getStateMachineData();
  data::Navigation nav_data           = data_.getNavigationData();
  data::Batteries batteries_data      = data_.getBatteriesData();
  data::Telemetry telemetry_data      = data_.getTelemetryData();
  data::Motors motors_data            = data_.getMotorData();

  bool encountered_failure = false;

  if (telemetry_data.emergency_stop_command) {
    encountered_failure = true;
    log_.ERR("STM", "STOP command received");
    telemetry_data.emergency_stop_command = false;
    data_.setTelemetryData(telemetry_data);
  } else if (nav_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in navigation");
  } else if (telemetry_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in telemetry");
  } else if (motors_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in motors");
  } else if (embrakes_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in embrakes");
  } else if (batteries_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in batteries");
  }

  if (encountered_failure) {
    sm_data.current_state = data::State::kFailureStopped;
    data_.setStateMachineData(sm_data);

    state_machine_->current_state_ = state_machine_->failure_stopped_;
  }
}

// Idling state

void Idling::transitionCheck()
{
  data::Telemetry telemetry_data = data_.getTelemetryData();

  if (telemetry_data.calibrate_command) {
    data::StateMachine sm_data   = data_.getStateMachineData();
    data::Navigation nav_data    = data_.getNavigationData();
    data::Batteries battery_data = data_.getBatteriesData();
    data::Sensors sensors_data   = data_.getSensorsData();
    data::Motors motors_data     = data_.getMotorData();
    // TODO: Only check the individual conditions if they haven't been found to be true before.
    // all modules must be initialised (except embrakes, they don't do that for some reason)
    bool modules_init = telemetry_data.module_status == ModuleStatus::kInit
                        && nav_data.module_status == ModuleStatus::kInit
                        && motors_data.module_status == ModuleStatus::kInit
                        && sensors_data.module_status == ModuleStatus::kInit
                        && battery_data.module_status == ModuleStatus::kInit;

    if (modules_init) {
      log_.INFO("STM", "calibrate command received and all modules initialised");
      telemetry_data.calibrate_command = false;
      data_.setTelemetryData(telemetry_data);
      log_.DBG("STM", "calibrate command cleared");

      sm_data.current_state = data::State::kCalibrating;
      data_.setStateMachineData(sm_data);

      state_machine_->current_state_ = state_machine_->calibrating_;
      log_.DBG("STM", "Transitioned to 'Calibrating'");
    }
  }
}

// Calibrating state

void Calibrating::transitionCheck()
{
  data::EmergencyBrakes embrakes_data = data_.getEmergencyBrakesData();
  data::StateMachine sm_data          = data_.getStateMachineData();
  data::Navigation nav_data           = data_.getNavigationData();
  data::Motors motors_data            = data_.getMotorData();

  // TODO: Only check the individual conditions if they haven't been found to be true before.
  // navigation, motors and embrakes must be ready
  bool modules_ready = nav_data.module_status == ModuleStatus::kReady
                       && motors_data.module_status == ModuleStatus::kReady
                       && embrakes_data.module_status == ModuleStatus::kReady;

  if (modules_ready) {
    log_.INFO("STM", "Calibration successful");

    sm_data.current_state = data::State::kReady;
    data_.setStateMachineData(sm_data);

    state_machine_->current_state_ = state_machine_->ready_;
    log_.DBG("STM", "Transitioned to 'Ready'");
  }
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

void Accelerating::checkEmergencyStop()
{
  data::EmergencyBrakes embrakes_data = data_.getEmergencyBrakesData();
  data::StateMachine sm_data          = data_.getStateMachineData();
  data::Navigation nav_data           = data_.getNavigationData();
  data::Batteries batteries_data      = data_.getBatteriesData();
  data::Telemetry telemetry_data      = data_.getTelemetryData();
  data::Motors motors_data            = data_.getMotorData();

  bool encountered_failure = false;

  if (telemetry_data.emergency_stop_command) {
    encountered_failure = true;
    log_.ERR("STM", "STOP command received");
    telemetry_data.emergency_stop_command = false;
    data_.setTelemetryData(telemetry_data);
  } else if (nav_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in navigation");
  } else if (telemetry_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in telemetry");
  } else if (motors_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in motors");
  } else if (embrakes_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in embrakes");
  } else if (batteries_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in batteries");
  }

  if (encountered_failure) {
    log_.ERR("STM", "Engaging emergency brakes");
    sm_data.current_state = data::State::kEmergencyBraking;
    data_.setStateMachineData(sm_data);
    state_machine_->current_state_ = state_machine_->failure_braking_;
  }
}

void Accelerating::transitionCheck()
{
  data::StateMachine sm_data     = data_.getStateMachineData();
  data::Navigation nav_data      = data_.getNavigationData();
  data::Telemetry telemetry_data = data_.getTelemetryData();

  if (telemetry_data.run_length <= nav_data.displacement + nav_data.braking_distance) {
    log_.INFO("STM", "max distance reached");
    log_.INFO("STM", "current distance: %fm, braking distance: %fm", nav_data.displacement,
              nav_data.braking_distance);

    sm_data.current_state = data::State::kNominalBraking;
    data_.setStateMachineData(sm_data);

    state_machine_->current_state_ = state_machine_->nominal_braking_;
    log_.DBG("STM", "Transitioned to 'Nominal Braking'");
  }
}

// Braking state

void NominalBraking::transitionCheck()
{
  data::Navigation nav_data  = data_.getNavigationData();
  data::StateMachine sm_data = data_.getStateMachineData();

  if (nav_data.velocity <= 0) {
    log_.INFO("STM", "zero velocity reached");

    sm_data.current_state = data::State::kFinished;
    data_.setStateMachineData(sm_data);

    state_machine_->current_state_ = state_machine_->finished_;
    log_.INFO("STM", "Transitioned to 'Finished'");
  }
}

void NominalBraking::checkEmergencyStop()
{
  data::EmergencyBrakes embrakes_data = data_.getEmergencyBrakesData();
  data::StateMachine sm_data          = data_.getStateMachineData();
  data::Navigation nav_data           = data_.getNavigationData();
  data::Batteries batteries_data      = data_.getBatteriesData();
  data::Telemetry telemetry_data      = data_.getTelemetryData();
  data::Motors motors_data            = data_.getMotorData();

  bool encountered_failure = false;

  if (telemetry_data.emergency_stop_command) {
    encountered_failure = true;
    log_.ERR("STM", "STOP command received");
    telemetry_data.emergency_stop_command = false;
    data_.setTelemetryData(telemetry_data);
  } else if (nav_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in navigation");
  } else if (telemetry_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in telemetry");
  } else if (motors_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in motors");
  } else if (embrakes_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in embrakes");
  } else if (batteries_data.module_status == ModuleStatus::kCriticalFailure) {
    encountered_failure = true;
    log_.ERR("STM", "Critical failure in batteries");
  }

  if (encountered_failure) {
    log_.ERR("STM", "Engaging emergency brakes");
    sm_data.current_state = data::State::kEmergencyBraking;
    data_.setStateMachineData(sm_data);
    state_machine_->current_state_ = state_machine_->failure_braking_;
  }
}

// Finished state

void Finished::transitionCheck()
{
  utils::System &sys             = utils::System::getSystem();
  data::Telemetry telemetry_data = data_.getTelemetryData();

  if (telemetry_data.shutdown_command) {
    log_.INFO("STM", "Shutdown command received");
    log_.INFO("STM", "System is shutting down");
    sys.running_ = false;
  }
}

void FailureBraking::transitionCheck()
{
  data::StateMachine sm_data     = data_.getStateMachineData();
  data::Navigation nav_data      = data_.getNavigationData();

  if (nav_data.velocity <= 0) {
    log_.INFO("STM", "zero velocity reached");

    sm_data.current_state = data::State::kFailureStopped;
    data_.setStateMachineData(sm_data);

    state_machine_->current_state_ = state_machine_->failure_stopped_;
    log_.INFO("STM", "Transitioned to 'FailureStopped'");
  }
}

void FailureStopped::transitionCheck()
{
  utils::System &sys             = utils::System::getSystem();
  data::Telemetry telemetry_data = data_.getTelemetryData();

  if (telemetry_data.shutdown_command) {
    log_.INFO("STM", "Shutdown command received");
    log_.INFO("STM", "System is shutting down");
    sys.running_ = false;
  }
}

}  // namespace state_machine
}  // namespace hyped
