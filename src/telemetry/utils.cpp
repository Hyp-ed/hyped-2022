/*
 * Author: Neil Weidinger
 * Organisation: HYPED
 * Date: March 2019
 * Description:
 *
 *    Copyright 2019 HYPED
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

#include "telemetry/utils.hpp"

namespace hyped {

namespace telemetry {

ProtoModuleStatus Utils::moduleStatusEnumConversion(DataModuleStatus status)
{
  // Proto Module Status enum namespace
  using pms = telemetry_data::ClientToServer;

  switch (status) {
    case data::ModuleStatus::kStart:
      return pms::START;
    case data::ModuleStatus::kInit:
      return pms::INIT;
    case data::ModuleStatus::kReady:
      return pms::READY;
    case data::ModuleStatus::kCriticalFailure:
      return pms::CRITICAL_FAILURE;
    default:
      // TODO(neil): throw error or something
      return pms::CRITICAL_FAILURE;
  }
}

ProtoState Utils::stateEnumConversion(DataState state)
{
  // Proto State Machine State enum namespace
  using psms = telemetry_data::ClientToServer::StateMachine;

  switch (state) {
    case data::State::kIdle:
      return psms::IDLE;
    case data::State::kCalibrating:
      return psms::CALIBRATING;
    case data::State::kReady:
      return psms::READY;
    case data::State::kAccelerating:
      return psms::ACCELERATING;
    case data::State::kNominalBraking:
      return psms::NOMINAL_BRAKING;
    case data::State::kEmergencyBraking:
      return psms::EMERGENCY_BRAKING;
    case data::State::kRunComplete:
      return psms::RUN_COMPLETE;
    case data::State::kFailureStopped:
      return psms::FAILURE_STOPPED;
    case data::State::kExiting:
      return psms::EXITING;
    case data::State::kFinished:
      return psms::FINISHED;
    default:
      // TODO(neil): throw error or something
      return psms::INVALID;
  }
}

}  // namespace telemetry
}  // namespace hyped
