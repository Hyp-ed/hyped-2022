/*
 * Author: Kornelija Sukyte, Franz Miltz, Efe Ozbatur
 * Organisation: HYPED
 * Date:
 * Description: The declared messages below are used in transitions while changing states.
 *    These declarations allow log messages to be seen and declared more effectively in the IDE. 
 *    stm --> Writes "STM" automatically before the log message
 *    m_ --> Shows the log messages as autocomplete options while typing (m for message)
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

class State; // Forward declaration

std::string stm_string;
const char *stm;

std::string stop_command;
const char *m_stop;

std::string critical_failure_navigation;
const char *m_critical_navigation;

std::string critical_failure_telemetry;
const char *m_critical_telemetry;

std::string critical_failure_motors;
const char *m_critical_motors;

std::string critical_failure_embrakes;
const char *m_critical_embrakes;

std::string critical_failure_batteries;
const char *m_critical_batteries;

std::string calibrate_initialised;
const char *m_calibrate_initialised;

std::string modules_calibrated;
const char *m_modules_calibrated;

std::string launch_command;
const char *m_launch_command;

std::string shutdown_command;
const char *m_shutdown_command;

std::string braking_zone;
const char *m_braking_zone;

std::string pod_stopped;
const char *m_pod_stopped;

}  // namespace state_machine

}  // namespace hyped

#endif  // STATE_MACHINE_MESSAGES_HPP_