#include "transitions.hpp"
namespace hyped {

namespace state_machine {

//--------------------------------------------------------------------------------------
// Emergency
//--------------------------------------------------------------------------------------

/*
 * @brief   Local function that determines whether or not there is an emergency.
 */
bool checkEmergency(Logger &log, EmergencyBrakes &embrakes_data, Navigation &nav_data,
                    Batteries &batteries_data, Telemetry &telemetry_data, Sensors &sensors_data,
                    Motors &motors_data)
{
  if (telemetry_data.emergency_stop_command) {
    log.ERR("STM", "stop command received");
    return true;
  } else if (nav_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR("STM", "critical failure in navigation");
    return true;
  } else if (telemetry_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR("STM", "critical failure in telemetry");
    return true;
  } else if (motors_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR("STM", "critical failure in motors");
    return true;
  } else if (embrakes_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR("STM", "critical failure in embrakes");
    return true;
  } else if (batteries_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR("STM", "critical failure in batteries");
    return true;
  } else if (sensors_data.module_status == ModuleStatus::kCriticalFailure) {
    log.ERR("STM", "critical failure in sensors");
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------
// Module Status
//--------------------------------------------------------------------------------------

bool checkModulesInitialised(Logger &log, EmergencyBrakes &embrakes_data, Navigation &nav_data,
                             Batteries &batteries_data, Telemetry &telemetry_data,
                             Sensors &sensors_data, Motors &motors_data)
{
  if (embrakes_data.module_status < ModuleStatus::kInit) return false;
  if (nav_data.module_status < ModuleStatus::kInit) return false;
  if (batteries_data.module_status < ModuleStatus::kInit) return false;
  if (telemetry_data.module_status < ModuleStatus::kInit) return false;
  if (sensors_data.module_status < ModuleStatus::kInit) return false;
  if (motors_data.module_status < ModuleStatus::kInit) return false;

  log.INFO("STM", "calibrate command received and all modules initialised");
  return true;
}

bool checkModulesReady(Logger &log, EmergencyBrakes &embrakes_data, Navigation &nav_data,
                       Batteries &batteries_data, Telemetry &telemetry_data, Sensors &sensors_data,
                       Motors &motors_data)
{
  if (embrakes_data.module_status != ModuleStatus::kReady) return false;
  if (nav_data.module_status != ModuleStatus::kReady) return false;
  if (batteries_data.module_status != ModuleStatus::kReady) return false;
  if (telemetry_data.module_status != ModuleStatus::kReady) return false;
  if (sensors_data.module_status != ModuleStatus::kReady) return false;
  if (motors_data.module_status != ModuleStatus::kReady) return false;

  log.INFO("STM", "all modules calibrated");
  return true;
}

//--------------------------------------------------------------------------------------
// Telemetry Commands
//--------------------------------------------------------------------------------------

bool checkCalibrateCommand(Logger &log, Telemetry &telemetry_data)
{
  if (!telemetry_data.calibrate_command) return false;

  return true;
}

bool checkLaunchCommand(Logger &log, Telemetry &telemetry_data)
{
  if (!telemetry_data.launch_command) return false;

  return true;
}

bool checkShutdownCommand(Logger &log, Telemetry &telemetry_data)
{
  if (!telemetry_data.shutdown_command) return false;

  return true;
}

//--------------------------------------------------------------------------------------
// Navigation Data Events
//--------------------------------------------------------------------------------------

bool checkEnteredBrakingZone(Logger &log, Navigation &nav_data)
{
  data::nav_t remaining_distance = Navigation::kRunLength - nav_data.displacement;
  data::nav_t required_distance  = nav_data.braking_distance + Navigation::kBrakingBuffer;
  if (remaining_distance > required_distance) return false;

  log.INFO("STM", "entered braking zone");
  return true;
}

bool checkReachedMaxVelocity(Logger &log, Navigation &nav_data)
{
  if (nav_data.velocity < Navigation::kMaximumVelocity) return false;

  log.INFO("STM", "reached maximum velocity");
  return true;
}

bool checkPodStopped(Logger &log, Navigation &nav_data)
{
  if (nav_data.velocity > 0) return false;

  log.INFO("STM", "pod has stopped");
  return true;
}

}  // namespace state_machine

}  // namespace hyped
