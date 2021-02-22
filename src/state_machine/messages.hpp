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

#include <string>

namespace hyped {

namespace state_machine {

// Declaration of log messages

struct Messages {
  // Identifies the module sending the message as STM
  static const char kStmLoggingIdentifier[];

  //--------------------------------------------------------------------------------------
  // Emergency
  //--------------------------------------------------------------------------------------

  // Sent upon receiving the stop command
  static const char kStopCommandLog[];

  // Sent upon encountering a critical failure in navigation
  static const char kCriticalNavigationLog[];

  // Sent upon encountering a critical failure in telemetry
  static const char kCriticalTelemetryLog[];

  // Sent upon encountering a critical failure in motors
  static const char kCriticalMotorsLog[];

  // Sent upon encountering a critical failure in embrakes
  static const char kCriticalEmbrakesLog[];

  // Sent upon encountering a critical failure in batteries
  static const char kCriticalBatteriesLog[];

  // Sent upon encountering a critical failure in sensors
  static const char kCriticalSensorsLog[];

  //--------------------------------------------------------------------------------------
  // Module Status
  //--------------------------------------------------------------------------------------

  // Sent upon determining that calibration command has been received
  // and all modules are initialised
  static const char kCalibrateInitialisedLog[];

  // Sent upon determining that all modules are calibrated
  static const char kModulesCalibratedLog[];

  //--------------------------------------------------------------------------------------
  // Navigation Data Events
  //--------------------------------------------------------------------------------------

  // Sent upon entering braking zone
  static const char kBrakingZoneLog[];

  // Sent upon determining that the pod has stopped
  static const char kPodStoppedLog[];

  //--------------------------------------------------------------------------------------
  // State Transitions
  //--------------------------------------------------------------------------------------

  // Sent upon shutting down system (entering Off state)
  static const char kShutdownLog[];

  // Sent upon entering a specific state
  static const char kEnteringStateFormat[];

  // Sent upon exiting a specific state
  static const char kExitingStateFormat[];

  // Sent upon exiting main loop
  static const char kExitingProgramFormat[];

  // Sent upon trying to transition from Off state
  static const char kTransitionFromOffLog[];

  // Messages only exists to hold static members, no constructor is needed.
  Messages() = delete;
};

}  // namespace state_machine

}  // namespace hyped

#endif  // STATE_MACHINE_MESSAGES_HPP_