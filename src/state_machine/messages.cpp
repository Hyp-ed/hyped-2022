/*
 * Author: Kornelija Sukyte, Franz Miltz
 * Organisation: HYPED
 * Date:
 * Description: Main is the state machine. This is where all the state is stored and how we interact
 * with the rest of HYPED. The SM only provides a framework though, the actual logic is implemented
 * in the other files.
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

#include "state_machine/messages.hpp"

namespace hyped {

namespace state_machine {

std::string stm_string = "STM";
const char *stm = stm_string.c_str();

std::string stop_command = "STOP command received";
const char *m_stop = stop_command.c_str();

std::string critical_failure_navigation = "Critical failure in navigation";
const char *m_critical_navigation = critical_failure_navigation.c_str();

std::string critical_failure_telemetry = "Critical failure in telemetry";
const char *m_critical_telemetry = critical_failure_telemetry.c_str();

std::string critical_failure_motors = "Critical failure in motors";
const char *m_critical_motors = critical_failure_motors.c_str();

std::string critical_failure_embr = "Critical failure in embrakes";
const char *m_critical_embrakes = critical_failure_embr.c_str();

std::string critical_failure_batteries = "Critical failure in batteries";
const char *m_critical_batteries = critical_failure_batteries.c_str();

std::string calibrate_initialised = "Calibrate command received and all modules initialised";
const char *m_calibrate_initialised = calibrate_initialised.c_str();

std::string modules_calibrated = "All modules calibrated";
const char *m_modules_calibrated = modules_calibrated.c_str();

std::string launch_command = "Launch command received";
const char *m_launch_command = launch_command.c_str();

std::string shutdown_command = "Shutdown command received";
const char *m_shutdown_command = shutdown_command.c_str();

std::string braking_zone = "Entered braking zone";
const char *m_braking_zone = braking_zone.c_str();

std::string pod_stopped = "The pod has stopped";
const char *m_pod_stopped = pod_stopped.c_str();


}  // namespace state_machine

}  // namespace hyped
