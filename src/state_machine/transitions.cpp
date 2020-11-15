/*
 * Author: Franz Miltz
 * Organisation: HYPED
 * Date:
 * Description:
 * Main instantiates HypedMachine. It also monitors other data and generates Events
 * for the HypedMachine. Note, StateMachine structure in Data is not updated here but
 * in HypedMachine.
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

#include "state_machine/transitions.hpp"

namespace hyped {

namespace state_machine {

//--------------------------------------------------------------------------------------
// Emergency
//--------------------------------------------------------------------------------------

/*
 * @brief   Local function that determines whether or not there is an emergency.
 */
inline bool checkEmergencyGeneral(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                                  Batteries batteries_data, Telemetry telemetry_data,
                                  Sensors sensors_data, Motors motors_data)
{
  if (telemetry_data.emergency_stop_command) {
    log.ERR("STM", "STOP command received");
    return true;
  } else if (nav_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR("STM", "Critical failure in navigation");
    return true;
  } else if (telemetry_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR("STM", "Critical failure in telemetry");
    return true;
  } else if (motors_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR("STM", "Critical failure in motors");
    return true;
  } else if (embrakes_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR("STM", "Critical failure in embrakes");
    return true;
  } else if (batteries_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR("STM", "Critical failure in batteries");
    return true;
  }
  return false;
}

State *checkEmergencyStationery(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                                Batteries batteries_data, Telemetry telemetry_data,
                                Sensors sensors_data, Motors motors_data)
{
  if (!checkEmergencyGeneral(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                             sensors_data, motors_data)) {
    return NULL;
  }
  return FailureStopped::getInstance();
}

State *checkEmergencyMoving(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                            Batteries batteries_data, Telemetry telemetry_data,
                            Sensors sensors_data, Motors motors_data)
{
  if (!checkEmergencyGeneral(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                             sensors_data, motors_data)) {
    return NULL;
  }
  return FailureBraking::getInstance();
}

//--------------------------------------------------------------------------------------
// Module Status
//--------------------------------------------------------------------------------------

State *checkModulesInitialised(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                               Batteries batteries_data, Telemetry telemetry_data,
                               Sensors sensors_data, Motors motors_data)
{
  if (!telemetry_data.calibrate_command) return NULL;

  if (embrakes_data.module_status != ModuleStatus::kInit) return NULL;
  if (nav_data.module_status != ModuleStatus::kInit) return NULL;
  if (batteries_data.module_status != ModuleStatus::kInit) return NULL;
  if (telemetry_data.module_status != ModuleStatus::kInit) return NULL;
  if (sensors_data.module_status != ModuleStatus::kInit) return NULL;
  if (motors_data.module_status != ModuleStatus::kInit) return NULL;

  log.INFO("STM", "Calibrate command received and all modules initialised");
  return Calibrating::getInstance();
}

State *checkModulesReady(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                         Motors motors_data)
{
  // We're only checking Navigation, Motors and Embrakes because only those modules are doing
  // calibration.
  if (embrakes_data.module_status != ModuleStatus::kReady) return NULL;
  if (nav_data.module_status != ModuleStatus::kReady) return NULL;
  if (motors_data.module_status != ModuleStatus::kReady) return NULL;

  log.INFO("STM", "All modules calibrated");
  return Ready::getInstance();
}

//--------------------------------------------------------------------------------------
// Telemetry Commands
//--------------------------------------------------------------------------------------

State *checkLaunchCommand(Logger &log, Telemetry telemetry_data)
{
  if (!telemetry_data.calibrate_command) return NULL;

  log.INFO("STM", "Launch command received");
  return Accelerating::getInstance();
}

State *checkShutdownCommand(Logger &log, Telemetry telemetry_data)
{
  if (!telemetry_data.shutdown_command) return NULL;

  log.INFO("STM", "Shutdown command received");
  return Off::getInstance();
}

//--------------------------------------------------------------------------------------
// Navigation Data Events
//--------------------------------------------------------------------------------------

State *checkEnteredBrakingZone(Logger &log, Navigation &nav_data, Telemetry &telemetry_data)
{
  // TODO(Franz): Fix braking buffer.
  if (telemetry_data.run_length > nav_data.displacement + nav_data.braking_distance) return NULL;

  log.INFO("STM", "Entered braking zone");
  return NominalBraking::getInstance();
}

/*
 * @brief    Returns true if the pod has stopped moving.
 */
inline bool checkPodStopped(Logger &log, Navigation &nav_data)
{
  if (nav_data.velocity > 0) return false;

  log.INFO("STM", "The pod has stopped");
  return true;
}

State *checkPodStoppedNominal(Logger &log, Navigation &nav_data)
{
  if (!checkPodStopped(log, nav_data)) return NULL;
  return Finished::getInstance();
}

State *checkPodStoppedEmergency(Logger &log, Navigation &nav_data)
{
  if (!checkPodStopped(log, nav_data)) return NULL;
  return FailureStopped::getInstance();
}

}  // namespace state_machine

}  // namespace hyped
