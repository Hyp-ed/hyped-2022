#include "transitions.hpp"

namespace hyped::demo_state_machine {

//--------------------------------------------------------------------------------------
// Emergency
//--------------------------------------------------------------------------------------

/*
 * @brief   Local function that determines whether or not there is an emergency.
 */
bool checkEmergency(utils::Logger &log, const data::EmergencyBrakes &brakes_data,
                    const data::Navigation &nav_data, const data::Batteries &batteries_data,
                    const data::Telemetry &telemetry_data, const data::Sensors &sensors_data,
                    const data::Motors &motors_data)
{
  if (nav_data.module_status == data::ModuleStatus::kCriticalFailure) {
    log.error("critical failure in navigation");
    return true;
  } else if (telemetry_data.module_status == data::ModuleStatus::kCriticalFailure) {
    log.error("critical failure in telemetry");
    return true;
  } else if (motors_data.module_status == data::ModuleStatus::kCriticalFailure) {
    log.error("critical failure in motors");
    return true;
  } else if (brakes_data.module_status == data::ModuleStatus::kCriticalFailure) {
    log.error("critical failure in brakes");
    return true;
  } else if (batteries_data.module_status == data::ModuleStatus::kCriticalFailure) {
    log.error("critical failure in batteries");
    return true;
  } else if (sensors_data.module_status == data::ModuleStatus::kCriticalFailure) {
    log.error("critical failure in sensors");
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------
// Module Status
//--------------------------------------------------------------------------------------

bool checkModulesInitialised(utils::Logger &log, const data::EmergencyBrakes &brakes_data,
                             const data::Navigation &nav_data,
                             const data::Batteries &batteries_data,
                             const data::Telemetry &telemetry_data,
                             const data::Sensors &sensors_data, const data::Motors &motors_data)
{
  if (brakes_data.module_status < data::ModuleStatus::kInit) return false;
  if (nav_data.module_status < data::ModuleStatus::kInit) return false;
  if (batteries_data.module_status < data::ModuleStatus::kInit) return false;
  if (telemetry_data.module_status < data::ModuleStatus::kInit) return false;
  if (sensors_data.module_status < data::ModuleStatus::kInit) return false;
  if (motors_data.module_status < data::ModuleStatus::kInit) return false;

  log.info("calibrate command received and all modules initialised");
  return true;
}

bool checkModulesReady(utils::Logger &log, const data::EmergencyBrakes &brakes_data,
                       const data::Navigation &nav_data, const data::Batteries &batteries_data,
                       const data::Telemetry &telemetry_data, const data::Sensors &sensors_data,
                       const data::Motors &motors_data)
{
  if (brakes_data.module_status != data::ModuleStatus::kReady) return false;
  if (nav_data.module_status != data::ModuleStatus::kReady) return false;
  if (batteries_data.module_status != data::ModuleStatus::kReady) return false;
  if (telemetry_data.module_status != data::ModuleStatus::kReady) return false;
  if (sensors_data.module_status != data::ModuleStatus::kReady) return false;
  if (motors_data.module_status != data::ModuleStatus::kReady) return false;

  log.info("all modules calibrated");
  return true;
}

//--------------------------------------------------------------------------------------
// Sensors Command
//--------------------------------------------------------------------------------------

bool checkHighPowerOff(const data::Sensors &sensors_data_struct)
{
  if (!sensors_data_struct.high_power_off) return false;

  return true;
}

//--------------------------------------------------------------------------------------
// Telemetry Commands
//--------------------------------------------------------------------------------------

bool checkCalibrateCommand(const data::Telemetry &telemetry_data)
{
  if (!telemetry_data.calibrate_command) return false;

  return true;
}

bool checkLaunchCommand(const data::Telemetry &telemetry_data)
{
  if (!telemetry_data.launch_command) return false;

  return true;
}

bool checkShutdownCommand(const data::Telemetry &telemetry_data)
{
  if (!telemetry_data.shutdown_command) return false;

  return true;
}

bool checkBrakingCommand(const data::Telemetry &telemetry_data)
{
  if (!telemetry_data.emergency_stop_command) return false;

  return true;
}

//--------------------------------------------------------------------------------------
// Navigation Data Events
//--------------------------------------------------------------------------------------

bool checkEnteredBrakingZone(utils::Logger &log, const data::Navigation &nav_data)
{
  data::nav_t remaining_distance = Navigation::kRunLength - nav_data.displacement;
  data::nav_t required_distance  = nav_data.braking_distance + Navigation::kBrakingBuffer;
  if (remaining_distance > required_distance) return false;

  log.info("entered braking zone");
  return true;
}

bool checkReachedMaxVelocity(utils::Logger &log, const data::Navigation &nav_data)
{
  if (nav_data.velocity < Navigation::kMaximumVelocity) return false;

  log.info("reached maximum velocity");
  return true;
}

bool checkPodStopped(utils::Logger &log, const data::Navigation &nav_data)
{
  if (nav_data.velocity > 0) return false;

  log.info("pod has stopped");
  return true;
}

}  // namespace hyped::demo_state_machine
