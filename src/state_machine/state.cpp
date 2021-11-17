#include "state.hpp"

namespace hyped {

namespace state_machine {

//--------------------------------------------------------------------------------------
//  General State
//--------------------------------------------------------------------------------------

State::State() : data_(data::Data::getInstance())
{
}

void State::updateModuleData()
{
  brakes_data_    = data_.getEmergencyBrakesData();
  nav_data_       = data_.getNavigationData();
  batteries_data_ = data_.getBatteriesData();
  telemetry_data_ = data_.getTelemetryData();
  sensors_data_   = data_.getSensorsData();
  motors_data_    = data_.getMotorData();
}

//--------------------------------------------------------------------------------------
//  Idle State
//--------------------------------------------------------------------------------------

Idle Idle::instance_;
data::State Idle::enum_value_       = data::kIdle;
char Idle::string_representation_[] = "Idle";

State *Idle::checkTransition(Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailureStopped::getInstance(); }

  bool all_initialised = checkModulesInitialised(log, brakes_data_, nav_data_, batteries_data_,
                                                 telemetry_data_, sensors_data_, motors_data_);
  if (all_initialised) { return PreCalibrating::getInstance(); }

  return nullptr;
}

//--------------------------------------------------------------------------------------
//  PreCalibrating
//--------------------------------------------------------------------------------------

PreCalibrating PreCalibrating::instance_;
data::State PreCalibrating::enum_value_       = data::kPreCalibrating;
char PreCalibrating::string_representation_[] = "PreCalibrating";

State *PreCalibrating::checkTransition(Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailureStopped::getInstance(); }

  bool calibrate_command = checkCalibrateCommand(telemetry_data_);
  if (calibrate_command) { return Calibrating::getInstance(); }

  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Calibrating
//--------------------------------------------------------------------------------------

Calibrating Calibrating::instance_;
data::State Calibrating::enum_value_       = data::kCalibrating;
char Calibrating::string_representation_[] = "Calibrating";

State *Calibrating::checkTransition(Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailureStopped::getInstance(); }

  bool all_ready = checkModulesReady(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                     sensors_data_, motors_data_);
  if (all_ready) { return Ready::getInstance(); }

  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Ready
//--------------------------------------------------------------------------------------

Ready Ready::instance_;
data::State Ready::enum_value_       = data::kReady;
char Ready::string_representation_[] = "Ready";

State *Ready::checkTransition(Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailureStopped::getInstance(); }

  bool recieved_launch_command = checkLaunchCommand(telemetry_data_);
  if (recieved_launch_command) { return Accelerating::getInstance(); }

  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Accelerating
//--------------------------------------------------------------------------------------

Accelerating Accelerating::instance_;
data::State Accelerating::enum_value_       = data::kAccelerating;
char Accelerating::string_representation_[] = "Accelerating";

State *Accelerating::checkTransition(Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailureBraking::getInstance(); }

  bool in_braking_zone = checkEnteredBrakingZone(log, nav_data_);
  if (in_braking_zone) { return NominalBraking::getInstance(); }

  bool reached_max_velocity = checkReachedMaxVelocity(log, nav_data_);
  if (reached_max_velocity) { return Cruising::getInstance(); }

  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Cruising
//--------------------------------------------------------------------------------------

Cruising Cruising::instance_;
data::State Cruising::enum_value_       = data::kCruising;
char Cruising::string_representation_[] = "Cruising";

State *Cruising::checkTransition(Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailureBraking::getInstance(); }

  bool in_braking_zone = checkEnteredBrakingZone(log, nav_data_);
  if (in_braking_zone) { return NominalBraking::getInstance(); }

  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Nominal Braking
//--------------------------------------------------------------------------------------

NominalBraking NominalBraking::instance_;
data::State NominalBraking::enum_value_       = data::kNominalBraking;
char NominalBraking::string_representation_[] = "NominalBraking";

State *NominalBraking::checkTransition(Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailureBraking::getInstance(); }

  bool stopped = checkPodStopped(log, nav_data_);
  if (stopped) { return Finished::getInstance(); }
  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Finished
//--------------------------------------------------------------------------------------

Finished Finished::instance_;
data::State Finished::enum_value_       = data::kFinished;
char Finished::string_representation_[] = "Finished";

State *Finished::checkTransition(Logger &log)
{
  // We only need to update telemetry data.
  telemetry_data_ = data_.getTelemetryData();

  bool received_shutdown_command = checkShutdownCommand(telemetry_data_);
  if (received_shutdown_command) { return Off::getInstance(); }
  return nullptr;
}

//--------------------------------------------------------------------------------------
//  FailureBraking
//--------------------------------------------------------------------------------------

FailureBraking FailureBraking::instance_;
data::State FailureBraking::enum_value_       = data::kEmergencyBraking;
char FailureBraking::string_representation_[] = "FailureBraking";

State *FailureBraking::checkTransition(Logger &log)
{
  // We only need to update navigation data.
  nav_data_ = data_.getNavigationData();

  bool stopped = checkPodStopped(log, nav_data_);
  if (stopped) { return FailureStopped::getInstance(); }
  return nullptr;
}

//--------------------------------------------------------------------------------------
//  FailureStopped
//--------------------------------------------------------------------------------------

FailureStopped FailureStopped::instance_;
data::State FailureStopped::enum_value_       = data::kFailureStopped;
char FailureStopped::string_representation_[] = "FailureStopped";

State *FailureStopped::checkTransition(Logger &log)
{
  // We only need to update telemetry data.
  telemetry_data_ = data_.getTelemetryData();

  bool received_shutdown_command = checkShutdownCommand(telemetry_data_);
  if (received_shutdown_command) { return Off::getInstance(); }
  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Off
//--------------------------------------------------------------------------------------

Off Off::instance_;

State *Off::checkTransition(Logger &log)
{
  log.ERR("STM", "tried to transition from Off state");
  return nullptr;
}

}  // namespace state_machine
}  // namespace hyped
