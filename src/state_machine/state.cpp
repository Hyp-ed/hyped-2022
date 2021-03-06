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
  brakes_data_    = data_.getBrakesData();
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
data::State Idle::enum_value_       = data::State::kIdle;
char Idle::string_representation_[] = "Idle";

State *Idle::checkTransition(utils::Logger &log)
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
data::State PreCalibrating::enum_value_       = data::State::kPreCalibrating;
char PreCalibrating::string_representation_[] = "PreCalibrating";

State *PreCalibrating::checkTransition(utils::Logger &log)
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
data::State Calibrating::enum_value_       = data::State::kCalibrating;
char Calibrating::string_representation_[] = "Calibrating";

State *Calibrating::checkTransition(utils::Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailureStopped::getInstance(); }

  bool all_ready = checkModulesReady(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                     sensors_data_, motors_data_);
  if (all_ready) { return PreReady::getInstance(); }

  return nullptr;
}

//--------------------------------------------------------------------------------------
//  PreReady
//--------------------------------------------------------------------------------------

PreReady PreReady::instance_;
data::State PreReady::enum_value_       = data::State::kPreReady;
char PreReady::string_representation_[] = "PreReady";

State *PreReady::checkTransition(Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailureStopped::getInstance(); }

  bool has_high_power_on = !checkHighPowerOff(sensors_data_);
  if (has_high_power_on) { return Ready::getInstance(); }

  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Ready
//--------------------------------------------------------------------------------------

Ready Ready::instance_;
data::State Ready::enum_value_       = data::State::kReady;
char Ready::string_representation_[] = "Ready";

State *Ready::checkTransition(utils::Logger &log)
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
data::State Accelerating::enum_value_       = data::State::kAccelerating;
char Accelerating::string_representation_[] = "Accelerating";

State *Accelerating::checkTransition(utils::Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailurePreBraking::getInstance(); }

  bool in_braking_zone = checkEnteredBrakingZone(log, nav_data_);
  if (in_braking_zone) { return PreBraking::getInstance(); }

  bool reached_max_velocity = checkReachedMaxVelocity(log, nav_data_);
  if (reached_max_velocity) { return Cruising::getInstance(); }

  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Cruising
//--------------------------------------------------------------------------------------

Cruising Cruising::instance_;
data::State Cruising::enum_value_       = data::State::kCruising;
char Cruising::string_representation_[] = "Cruising";

State *Cruising::checkTransition(utils::Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailurePreBraking::getInstance(); }

  bool in_braking_zone = checkEnteredBrakingZone(log, nav_data_);
  if (in_braking_zone) { return PreBraking::getInstance(); }

  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Pre-Braking
//--------------------------------------------------------------------------------------

PreBraking PreBraking::instance_;
data::State PreBraking::enum_value_       = data::State::kPreBraking;
char PreBraking::string_representation_[] = "PreBraking";

State *PreBraking::checkTransition(utils::Logger &log)
{
  updateModuleData();

  bool emergency = checkEmergency(log, brakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (emergency) { return FailurePreBraking::getInstance(); }

  bool has_high_power_off = checkHighPowerOff(sensors_data_);
  if (has_high_power_off) { return NominalBraking::getInstance(); }
  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Nominal Braking
//--------------------------------------------------------------------------------------

NominalBraking NominalBraking::instance_;
data::State NominalBraking::enum_value_       = data::State::kNominalBraking;
char NominalBraking::string_representation_[] = "NominalBraking";

State *NominalBraking::checkTransition(utils::Logger &log)
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
data::State Finished::enum_value_       = data::State::kFinished;
char Finished::string_representation_[] = "Finished";

State *Finished::checkTransition(utils::Logger &)
{
  // We only need to update telemetry data.
  telemetry_data_ = data_.getTelemetryData();

  bool received_shutdown_command = checkShutdownCommand(telemetry_data_);
  if (received_shutdown_command) { return Off::getInstance(); }
  return nullptr;
}

//--------------------------------------------------------------------------------------
//  Failure Pre-Braking
//--------------------------------------------------------------------------------------

FailurePreBraking FailurePreBraking::instance_;
data::State FailurePreBraking::enum_value_       = data::State::kFailurePreBraking;
char FailurePreBraking::string_representation_[] = "FailurePreBraking";

State *FailurePreBraking::checkTransition(utils::Logger &)
{
  updateModuleData();

  bool has_high_power_off = checkHighPowerOff(sensors_data_);
  if (has_high_power_off) { return FailureBraking::getInstance(); }
  return nullptr;
}

//--------------------------------------------------------------------------------------
//  FailureBraking
//--------------------------------------------------------------------------------------

FailureBraking FailureBraking::instance_;
data::State FailureBraking::enum_value_       = data::State::kFailureBraking;
char FailureBraking::string_representation_[] = "FailureBraking";

State *FailureBraking::checkTransition(utils::Logger &log)
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
data::State FailureStopped::enum_value_       = data::State::kFailureStopped;
char FailureStopped::string_representation_[] = "FailureStopped";

State *FailureStopped::checkTransition(utils::Logger &)
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

State *Off::checkTransition(utils::Logger &log)
{
  log.error("tried to transition from Off state");
  return nullptr;
}

}  // namespace state_machine
}  // namespace hyped
