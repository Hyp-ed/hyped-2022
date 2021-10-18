#pragma once

#include "state.hpp"

#include <data/data.hpp>
#include <utils/logger.hpp>

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
bool checkEmergency(Logger &log, const EmergencyBrakes &brakes_data, const Navigation &nav_data,
                    const Batteries &batteries_data, const Telemetry &telemetry_data, const Sensors &sensors_data,
                    const Motors &motors_data);

//--------------------------------------------------------------------------------------
// Module Status
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns true iff the command has been received and all modules are
 *          initialised.
 */
bool checkModulesInitialised(Logger &log, const EmergencyBrakes &brakes_data, const Navigation &nav_data,
                             const Batteries &batteries_data, const Telemetry &telemetry_data,
                             const Sensors &sensors_data, const Motors &motors_data);

/*
 * @brief    Returns true iff all modules are ready.
 */
bool checkModulesReady(Logger &log, const EmergencyBrakes &brakes_data, const Navigation &nav_data,
                       const Batteries &batteries_data, const Telemetry &telemetry_data, const Sensors &sensors_data,
                       const Motors &motors_data);

//--------------------------------------------------------------------------------------
// Telemetry Commands
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns true iff the calibrate command has been received.
 */
bool checkCalibrateCommand(const Telemetry &telemetry_data);

/*
 * @brief   Returns true iff the launch command has been received.
 */
bool checkLaunchCommand(const Telemetry &telemetry_data);

/*
 * @brief    Returns true iff the shutdown command has been received.
 */
bool checkShutdownCommand(const Telemetry &telemetry_data);

//--------------------------------------------------------------------------------------
// Navigation Data Events
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns true iff the pod is close enough to the end of the track.
 */
bool checkEnteredBrakingZone(Logger &log, const Navigation &nav_data);

/*
 * @brief   Returns true iff the pod has reached the maximum velocity.
 */
bool checkReachedMaxVelocity(Logger &log, const Navigation &nav_data);

/*
 * @brief   Returns true iff the pod has reached zero velocity.
 */
bool checkPodStopped(Logger &log, const Navigation &nav_data);
}  // namespace state_machine

}  // namespace hyped
