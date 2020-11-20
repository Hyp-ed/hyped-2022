
/*
 * Authors: Kornelija Sukyte, Franz Miltz, Efe Ozbatur, Yining Wang
 * Organisation: HYPED
 * Date:
 * Description:
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
#include "state_machine/state.hpp"

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
  embrakes_data_  = data_.getEmergencyBrakesData();
  nav_data_       = data_.getNavigationData();
  batteries_data_ = data_.getBatteriesData();
  telemetry_data_ = data_.getTelemetryData();
  sensors_data_   = data_.getSensorsData();
  motors_data_    = data_.getMotorData();
}

//--------------------------------------------------------------------------------------
//  Idling State
//--------------------------------------------------------------------------------------

Idling *Idling::instance_ = new Idling();

State *Idling::checkTransition(Logger &log)
{
  updateModuleData();

  State *next;
  next = checkEmergencyStationary(log, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (next) return next;

  next = checkModulesInitialised(log, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                 sensors_data_, motors_data_);

  if (next) return next;
  return nullptr;
}

void Idling::enter(Logger &log)
{
  log.INFO("STM", "Entering Idling state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kIdle;
  data_.setStateMachineData(sm_data);
}

void Idling::exit(Logger &log)
{
  log.INFO("STM", "Exiting Idling state");
}

//--------------------------------------------------------------------------------------
//  Calibrating
//--------------------------------------------------------------------------------------

Calibrating *Calibrating::instance_ = new Calibrating();

State *Calibrating::checkTransition(Logger &log)
{
  updateModuleData();

  State *next;
  next = checkEmergencyStationary(log, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (next) return next;

  next = checkModulesReady(log, embrakes_data_, nav_data_, motors_data_);
  if (next) return next;

  return nullptr;
}

void Calibrating::enter(Logger &log)
{
  log.INFO("STM", "Entering Calibrating state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kCalibrating;
  data_.setStateMachineData(sm_data);
}

void Calibrating::exit(Logger &log)
{
  log.INFO("STM", "Exiting Calibrating state");
}

//--------------------------------------------------------------------------------------
//  Ready
//--------------------------------------------------------------------------------------

Ready *Ready::instance_ = new Ready();

State *Ready::checkTransition(Logger &log)
{
  updateModuleData();

  State *next;

  next = checkEmergencyStationary(log, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (next) return next;

  next = checkLaunchCommand(log, telemetry_data_);
  if (next) return next;

  return nullptr;
}

void Ready::enter(Logger &log)
{
  log.INFO("STM", "Entering Ready state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kReady;
  data_.setStateMachineData(sm_data);
}

void Ready::exit(Logger &log)
{
  log.INFO("STM", "Exiting Ready state");
}

//--------------------------------------------------------------------------------------
//  Accelerating
//--------------------------------------------------------------------------------------

Accelerating *Accelerating::instance_ = new Accelerating();

State *Accelerating::checkTransition(Logger &log)
{
  updateModuleData();

  State *next;

  next = checkEmergencyMoving(log, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                              sensors_data_, motors_data_);
  if (next) return next;

  next = checkEnteredBrakingZone(log, nav_data_);
  if (next) return next;

  return nullptr;
}

void Accelerating::enter(Logger &log)
{
  log.INFO("STM", "Entering Accelerating state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kAccelerating;
  data_.setStateMachineData(sm_data);
}

void Accelerating::exit(Logger &log)
{
  log.INFO("STM", "Exiting Accelerating state");
}

//--------------------------------------------------------------------------------------
//  Nominal Braking
//--------------------------------------------------------------------------------------

NominalBraking *NominalBraking::instance_ = new NominalBraking();

State *NominalBraking::checkTransition(Logger &log)
{
  updateModuleData();

  State *next;

  next = checkEmergencyMoving(log, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                              sensors_data_, motors_data_);
  if (next) return next;

  next = checkPodStoppedNominal(log, nav_data_);
  if (next) return next;
  return nullptr;
}

void NominalBraking::enter(Logger &log)
{
  log.INFO("STM", "Entering NominalBraking state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kNominalBraking;
  data_.setStateMachineData(sm_data);
}

void NominalBraking::exit(Logger &log)
{
  log.INFO("STM", "Exiting NominalBraking state");
}

//--------------------------------------------------------------------------------------
//  Finished
//--------------------------------------------------------------------------------------

Finished *Finished::instance_ = new Finished();

State *Finished::checkTransition(Logger &log)
{
  // We only need to update telemetry data.
  telemetry_data_ = data_.getTelemetryData();
  return checkShutdownCommand(log, telemetry_data_);
}

void Finished::enter(Logger &log)
{
  log.INFO("STM", "Entering Finished state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kFinished;
  data_.setStateMachineData(sm_data);
}

void Finished::exit(Logger &log)
{
  log.INFO("STM", "Exiting Finished state");
}

//--------------------------------------------------------------------------------------
//  FailureBraking
//--------------------------------------------------------------------------------------

FailureBraking *FailureBraking::instance_ = new FailureBraking();

State *FailureBraking::checkTransition(Logger &log)
{
  // We only need to update navigation data.
  nav_data_ = data_.getNavigationData();
  return checkPodStoppedEmergency(log, nav_data_);
}

void FailureBraking::enter(Logger &log)
{
  log.INFO("STM", "Entering FailureBraking state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kEmergencyBraking;
  data_.setStateMachineData(sm_data);
}

void FailureBraking::exit(Logger &log)
{
  log.INFO("STM", "Exiting FailureBraking state");
}

//--------------------------------------------------------------------------------------
//  FailureStopped
//--------------------------------------------------------------------------------------

FailureStopped *FailureStopped::instance_ = new FailureStopped();

State *FailureStopped::checkTransition(Logger &log)
{
  // We only need to update telemetry data.
  telemetry_data_ = data_.getTelemetryData();
  return checkShutdownCommand(log, telemetry_data_);
}

void FailureStopped::enter(Logger &log)
{
  log.INFO("STM", "Entering FailureStopped state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kFailureStopped;
  data_.setStateMachineData(sm_data);
}

void FailureStopped::exit(Logger &log)
{
  log.INFO("STM", "Exiting FailureStopped state");
}

//--------------------------------------------------------------------------------------
//  Off
//--------------------------------------------------------------------------------------

Off *Off::instance_ = new Off();

State *Off::checkTransition(Logger &log)
{
  // This function should never be called.
  log.ERR("STM", "Tried to transition from Off state");
  return nullptr;
}

void Off::enter(Logger &log)
{
  utils::System &sys = utils::System::getSystem();
  log.INFO("STM", "System is shutting down");
  sys.running_ = false;
}

void Off::exit(Logger &log)
{
  // We never exit this state anyways...
}

}  // namespace state_machine
}  // namespace hyped
