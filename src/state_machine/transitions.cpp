/*
 * Author: Franz Miltz
 * Organisation: HYPED
 * Date:
 * Description: We implement the transition conditions declared in transitions.hpp based on the
 * desired behaviour.
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

#include "transitions.hpp"

#include "messages.hpp"

namespace hyped {

namespace state_machine {

//--------------------------------------------------------------------------------------
// Emergency
//--------------------------------------------------------------------------------------

/*
 * @brief   Local function that determines whether or not there is an emergency.
 */
bool checkEmergency(Logger &log, EmergencyBrakes &embrakes_data, Navigation &nav_data,
                    Batteries &batteries_data, Telemetry &telemetry_data, Sensors &sensors_data,
                    Motors &motors_data)
{
  if (telemetry_data.emergency_stop_command) {
    log.ERR(Messages::kStmLoggingIdentifier, Messages::kStopCommandLog);
    return true;
  } else if (nav_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR(Messages::kStmLoggingIdentifier, Messages::kCriticalNavigationLog);
    return true;
  } else if (telemetry_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR(Messages::kStmLoggingIdentifier, Messages::kCriticalTelemetryLog);
    return true;
  } else if (motors_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR(Messages::kStmLoggingIdentifier, Messages::kCriticalMotorsLog);
    return true;
  } else if (embrakes_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR(Messages::kStmLoggingIdentifier, Messages::kCriticalEmbrakesLog);
    return true;
  } else if (batteries_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR(Messages::kStmLoggingIdentifier, Messages::kCriticalBatteriesLog);
    return true;
  } else if (sensors_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR(Messages::kStmLoggingIdentifier, Messages::kCriticalSensorsLog);
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------
// Module Status
//--------------------------------------------------------------------------------------

bool checkModulesInitialised(Logger &log, EmergencyBrakes &embrakes_data, Navigation &nav_data,
                             Batteries &batteries_data, Telemetry &telemetry_data,
                             Sensors &sensors_data, Motors &motors_data)
{
  if (embrakes_data.module_status < ModuleStatus::kInit) return false;
  if (nav_data.module_status < ModuleStatus::kInit) return false;
  if (batteries_data.module_status < ModuleStatus::kInit) return false;
  if (telemetry_data.module_status < ModuleStatus::kInit) return false;
  if (sensors_data.module_status < ModuleStatus::kInit) return false;
  if (motors_data.module_status < ModuleStatus::kInit) return false;

  log.INFO(Messages::kStmLoggingIdentifier, Messages::kCalibrateInitialisedLog);
  return true;
}

bool checkModulesReady(Logger &log, EmergencyBrakes &embrakes_data, Navigation &nav_data,
                       Batteries &batteries_data, Telemetry &telemetry_data, Sensors &sensors_data,
                       Motors &motors_data)
{
  if (embrakes_data.module_status != ModuleStatus::kReady) return false;
  if (nav_data.module_status != ModuleStatus::kReady) return false;
  if (batteries_data.module_status != ModuleStatus::kReady) return false;
  if (telemetry_data.module_status != ModuleStatus::kReady) return false;
  if (sensors_data.module_status != ModuleStatus::kReady) return false;
  if (motors_data.module_status != ModuleStatus::kReady) return false;

  log.INFO(Messages::kStmLoggingIdentifier, Messages::kModulesCalibratedLog);
  return true;
}

//--------------------------------------------------------------------------------------
// Telemetry Commands
//--------------------------------------------------------------------------------------

bool checkCalibrateCommand(Logger &log, Telemetry &telemetry_data)
{
  if (!telemetry_data.calibrate_command) return false;

  return true;
}

bool checkLaunchCommand(Logger &log, Telemetry &telemetry_data)
{
  if (!telemetry_data.launch_command) return false;

  return true;
}

bool checkShutdownCommand(Logger &log, Telemetry &telemetry_data)
{
  if (!telemetry_data.shutdown_command) return false;

  return true;
}

//--------------------------------------------------------------------------------------
// Navigation Data Events
//--------------------------------------------------------------------------------------

bool checkEnteredBrakingZone(Logger &log, Navigation &nav_data)
{
  data::nav_t remaining_distance = Navigation::kRunLength - nav_data.displacement;
  data::nav_t required_distance  = nav_data.braking_distance + Navigation::kBrakingBuffer;
  if (remaining_distance > required_distance) return false;

  log.INFO(Messages::kStmLoggingIdentifier, Messages::kBrakingZoneLog);
  return true;
}

bool checkReachedMaxVelocity(Logger &log, Navigation &nav_data)
{
  if (nav_data.velocity < Navigation::kMaximumVelocity) return false;

  log.INFO(Messages::kStmLoggingIdentifier, Messages::kMaxVelocityLog);
  return true;
}

bool checkPodStopped(Logger &log, Navigation &nav_data)
{
  if (nav_data.velocity > 0) return false;

  log.INFO(Messages::kStmLoggingIdentifier, Messages::kPodStoppedLog);
  return true;
}

}  // namespace state_machine

}  // namespace hyped
