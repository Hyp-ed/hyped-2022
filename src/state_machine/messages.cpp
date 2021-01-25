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

// List of log messages

constexpr char *kStmLoggingIdentifier = "STM";

constexpr char *kStopLog = "STOP command received";

constexpr char *kCriticalNavigationLog = "Critical failure in navigation";

constexpr char *kCriticalTelemetryLog = "Critical failure in telemetry";

constexpr char *kCriticalMotorsLog = "Critical failure in motors";

constexpr char *kCriticalEmbrakesLog = "Critical failure in embrakes";

constexpr char *kCriticalBatteriesLog = "Critical failure in batteries";

constexpr char *kCalibrateInitialisedLog = "Calibrate command received and all modules initialised";

constexpr char *kModulesCalibratedLog = "All modules calibrated";

constexpr char *kLaunchCommandLog = "Launch command received";

constexpr char *kShutdownCommandLog = "Shutdown command received";

constexpr char *kBrakingZoneLog = "Entered braking zone";

constexpr char *kPodStoppedLog = "The pod has stopped";


}  // namespace state_machine

}  // namespace hyped
