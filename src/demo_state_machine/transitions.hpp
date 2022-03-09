#pragma once

#include "state.hpp"

#include <data/data.hpp>
#include <utils/logger.hpp>

namespace hyped {

namespace demo_state_machine {

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
bool checkEmergency(Logger &log, const data::EmergencyBrakes &brakes_data,
                    const data::Navigation &nav_data, const data::Batteries &batteries_data,
                    const data::Telemetry &telemetry_data, const data::Sensors &sensors_data,
                    const data::Motors &motors_data);

//--------------------------------------------------------------------------------------
// Module Status
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns true iff the command has been received and all modules are
 *          initialised.
 */
bool checkModulesInitialised(Logger &log, const data::EmergencyBrakes &brakes_data,
                             const data::Navigation &nav_data,
                             const data::Batteries &batteries_data,
                             const data::Telemetry &telemetry_data,
                             const data::Sensors &sensors_data, const data::Motors &motors_data);

/*
 * @brief    Returns true iff all modules are ready.
 */
bool checkModulesReady(Logger &log, const data::EmergencyBrakes &brakes_data,
                       const data::Navigation &nav_data, const data::Batteries &batteries_data,
                       const data::Telemetry &telemetry_data, const data::Sensors &sensors_data,
                       const data::Motors &motors_data);

//--------------------------------------------------------------------------------------
// Sensors Command
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns true iff all SSRs have HP off.
 */
bool checkHighPowerOff(const data::Sensors &sensors_data_struct);

//--------------------------------------------------------------------------------------
// Telemetry Commands
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns true iff the calibrate command has been received.
 */
bool checkCalibrateCommand(const data::Telemetry &telemetry_data);

/*
 * @brief   Returns true iff the launch command has been received.
 */
bool checkLaunchCommand(const data::Telemetry &telemetry_data);

/*
 * @brief    Returns true iff the shutdown command has been received.
 */
bool checkShutdownCommand(const data::Telemetry &telemetry_data);

//--------------------------------------------------------------------------------------
// Navigation Data Events
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns true iff the pod is close enough to the end of the track.
 */
bool checkEnteredBrakingZone(Logger &log, const data::Navigation &nav_data);

/*
 * @brief   Returns true iff the pod has reached the maximum velocity.
 */
bool checkReachedMaxVelocity(Logger &log, const data::Navigation &nav_data);

/*
 * @brief   Returns true iff the pod has reached zero velocity.
 */
bool checkPodStopped(Logger &log, const data::Navigation &nav_data);
}  // namespace demo_state_machine

}  // namespace hyped
