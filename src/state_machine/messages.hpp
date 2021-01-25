/*
 * Author: Kornelija Sukyte, Franz Miltz, Efe Ozbatur
 * Organisation: HYPED
 * Date:
 * Description: The declared messages below are used in transitions while changing states.
 *    These declarations allow log messages to be seen and declared more effectively in the IDE. 
 *    Refer to "state_machine/messages.cpp" to see the log messages.
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

#ifndef STATE_MACHINE_MESSAGES_HPP_
#define STATE_MACHINE_MESSAGES_HPP_

#include "data/data.hpp"
#include "state_machine/main.hpp"
#include "state_machine/transitions.hpp"
#include "state_machine/state.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/timer.hpp"
#include <string>

namespace hyped {

using data::ModuleStatus;
using utils::Logger;
using utils::concurrent::Thread;

namespace state_machine {

// Declaration of log messages

constexpr char *kStmLoggingIdentifier;

constexpr char *kStopLog;

constexpr char *kCriticalNavigationLog;

constexpr char *kCriticalTelemetryLog;

constexpr char *kCriticalMotorsLog;

constexpr char *kCriticalEmbrakesLog;

constexpr char *kCriticalBatteriesLog;

constexpr char *kCalibrateInitialisedLog;

constexpr char *kModulesCalibratedLog;

constexpr char *kLaunchCommandLog;

constexpr char *kShutdownCommandLog;

constexpr char *kBrakingZoneLog;

constexpr char *kPodStoppedLog;

}  // namespace state_machine

}  // namespace hyped

#endif  // STATE_MACHINE_MESSAGES_HPP_