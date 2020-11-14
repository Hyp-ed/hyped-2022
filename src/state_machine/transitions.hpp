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
 * @brief   Returns the FailureStopped if there's an emergency and NULL otherwise.
 */
State *checkEmergencyStationery(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                                Batteries batteries_data, Telemetry telemetry_data,
                                Sensors sensors_data, Motors motors_data);

/*
 * @brief   Returns the FailureBraking if there's an emergency and NULL otherwise.
 */
State *checkEmergencyMoving(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                            Batteries batteries_data, Telemetry telemetry_data,
                            Sensors sensors_data, Motors motors_data);

//--------------------------------------------------------------------------------------
// Module Status
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns the Calibrating state if the command has been received and all modules are
 *          initialised.
 */
State *checkModulesInitialised(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                               Batteries batteries_data, Telemetry telemetry_data,
                               Sensors sensors_data, Motors motors_data);

/*
 * @brief    Returns the Ready state if all Embrakes, Navigation and Motors are ready.
 */
State *checkModulesReady(Logger &log, EmergencyBrakes embrakes_data, Navigation nav_data,
                         Motors motors_data);

//--------------------------------------------------------------------------------------
// Telemetry Commands
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns the Accelerating state if the launch command has been received.
 */
State *checkLaunchCommand(Logger &log, Telemetry telemetry_data);

/*
 * @brief    Returns the Stopped state if the shutdown command has been received.
 */
State *checkShutdownCommand(Logger &log, Telemetry telemetry_data);

//--------------------------------------------------------------------------------------
// Navigation Data Events
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns the Braking state if the pod is close enough to the end of the track.
 */
State *checkEnteredBrakingZone(Logger &log, Navigation &nav_data, Telemetry &telemetry_data);

/*
 * @brief   Returns the Finished state if the pod has stopped and NULL otherwise.
 */
State *checkPodStoppedNominal(Logger &log, Navigation &nav_data);

/*
 * @brief   Returns the FailureStopped state if the pod has stopped and NULL otherwise.
 */
State *checkPodStoppedEmergency(Logger &log, Navigation &nav_data);

}  // namespace state_machine

}  // namespace hyped

#endif  // STATE_MACHINE_TRANSITIONS_HPP_
