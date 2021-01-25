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

// Identifies the module sending the message as STM
const char Messages::kStmLoggingIdentifier[] = "STM";

// Message upon receiving the stop command
const char Messages::kStopCommandLog[] = "STOP command received";

const char Messages::kCriticalNavigationLog[] = "Critical failure in navigation";

const char Messages::kCriticalTelemetryLog[] = "Critical failure in telemetry";

const char Messages::kCriticalMotorsLog[] = "Critical failure in motors";

const char Messages::kCriticalEmbrakesLog[] = "Critical failure in embrakes";

const char Messages::kCriticalBatteriesLog[] = "Critical failure in batteries";

const char Messages::kCalibrateInitialisedLog[]
  = "Calibrate command received and all modules initialised";

const char Messages::kModulesCalibratedLog[] = "All modules calibrated";

const char Messages::kLaunchCommandLog[] = "Launch command received";

const char Messages::kShutdownCommandLog[] = "Shutdown command received";

const char Messages::kBrakingZoneLog[] = "Entered braking zone";

const char Messages::kPodStoppedLog[] = "The pod has stopped";

}  // namespace state_machine

}  // namespace hyped
