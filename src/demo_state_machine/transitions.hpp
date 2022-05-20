#pragma once

#include "state.hpp"

#include <data/data.hpp>
#include <utils/logger.hpp>

namespace hyped::demo_state_machine {

using hyped::data::Brakes;
using hyped::data::FullBatteryData;
using hyped::data::Motors;
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
bool checkEmergency(Logger &log, const data::Brakes &brakes_data,  const data::FullBatteryData &batteries_data, 
                    const data::Sensors &sensors_data,
                    const data::Motors &motors_data);

//--------------------------------------------------------------------------------------
// Module Status
//--------------------------------------------------------------------------------------

/*
 * @brief   Returns true iff the command has been received and all modules are
 *          initialised.
 */
bool checkModulesInitialised(Logger &log, const data::Brakes &brakes_data,
                             const data::FullBatteryData &batteries_data,
                             const data::Sensors &sensors_data, const data::Motors &motors_data);

/*
 * @brief    Returns true iff all modules are ready.
 */
bool checkModulesReady(Logger &log, const data::Brakes &brakes_data,
                       const data::FullBatteryData &batteries_data,
                       const data::Sensors &sensors_data, const data::Motors &motors_data);

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

/*
 * @brief    Returns true iff the braking command has been received.
 */
bool checkBrakingCommand(const data::Telemetry &telemetry_data);

/*
 * @brief   Returns true iff the stop command has been received. 
 */
bool checkStopCommand(Logger &log, const data::Telemetry &telemetry_data);

}  // namespace hyped::demo_state_machine
