/*
 * Author: Franz Miltz
 * Organisation: HYPED
 * Date:
 * Description: Here we declare helper functions to determine, based on specific inputs, wether a
 * condition is met. Those conditions can be very simple or very complicated. We also log notable
 * observations that have been made.
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

#ifndef STATE_MACHINE_TRANSITIONS_HPP_
#define STATE_MACHINE_TRANSITIONS_HPP_

#include "data/data.hpp"
#include "state_machine/state.hpp"
#include "utils/logger.hpp"

namespace hyped {

namespace state_machine {

using hyped::data::Batteries;
using hyped::data::EmergencyBrakes;
using hyped::data::Motors;
using hyped::data::Navigation;
using hyped::data::Sensors;
using hyped::data::Telemetry;
using utils::Logger;

class State;  // Forward declaration

//--------------------------------------------------------------------------------------
// Emergency
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns the FailureStopped if there's an emergency and nullptr otherwise.
 */
bool checkEmergency(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                    Batteries batteries_data, Telemetry telemetry_data, Sensors sensors_data,
                    Motors motors_data);

//--------------------------------------------------------------------------------------
// Module Status
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns true iff the command has been received and all modules are
 *          initialised.
 */
bool checkModulesInitialised(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                             Batteries batteries_data, Telemetry telemetry_data,
                             Sensors sensors_data, Motors motors_data);

/*
 * @brief    Returns true iff all of Embrakes, Navigation and Motors are ready.
 */
bool checkModulesReady(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                       Motors motors_data);

//--------------------------------------------------------------------------------------
// Telemetry Commands
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns true iff the calibrate command has been received.
 */
bool checkCalibrateCommand(Logger &log, Telemetry telemetry_data);

/*
 * @brief   Returns true iff the launch command has been received.
 */
bool checkLaunchCommand(Logger &log, Telemetry telemetry_data);

/*
 * @brief    Returns true iff the shutdown command has been received.
 */
bool checkShutdownCommand(Logger &log, Telemetry telemetry_data);

//--------------------------------------------------------------------------------------
// Navigation Data Events
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns true iff the pod is close enough to the end of the track.
 */
bool checkEnteredBrakingZone(Logger &log, Navigation &nav_data);

/*
 * @brief   Returns true iff the pod has reached zero velocity.
 */
bool checkPodStopped(Logger &log, Navigation &nav_data);
}  // namespace state_machine

}  // namespace hyped

#endif  // STATE_MACHINE_TRANSITIONS_HPP_
