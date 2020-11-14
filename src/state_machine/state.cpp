
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

State::State(Logger &log, Main *state_machine)
    : log_(log),
      data_(data::Data::getInstance()),
      state_machine_(state_machine)
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

Idling *Idling::instance_;

State *Idling::checkTransition()
{
  updateModuleData();

  State *next;
  next = checkEmergencyStationery(log_, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (next) return next;

  next = checkModulesInitialised(log_, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                 sensors_data_, motors_data_);

  if (next) return next;
  return NULL;
}

void Idling::enter()
{
  log_.INFO("STM", "Entering Idling state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kIdle;
  data_.setStateMachineData(sm_data);
}

void Idling::exit()
{
  log_.INFO("STM", "Exiting Idling state");
}

//--------------------------------------------------------------------------------------
//  Calibrating
//--------------------------------------------------------------------------------------

Calibrating *Calibrating::instance_;

State *Calibrating::checkTransition()
{
  updateModuleData();

  State *next;
  next = checkEmergencyStationery(log_, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (next) return next;

  next = checkModulesReady(log_, embrakes_data_, nav_data_, motors_data_);
  if (next) return next;

  return NULL;
}

void Calibrating::enter()
{
  log_.INFO("STM", "Entering Calibrating state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kCalibrating;
  data_.setStateMachineData(sm_data);
}

void Calibrating::exit()
{
  log_.INFO("STM", "Exiting Calibrating state");
}

//--------------------------------------------------------------------------------------
//  Ready
//--------------------------------------------------------------------------------------

Ready *Ready::instance_;

State *Ready::checkTransition()
{
  updateModuleData();

  State *next;

  next = checkEmergencyStationery(log_, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
  if (next) return next;

  next = checkLaunchCommand(log_, telemetry_data_);
  if (next) return next;

  return NULL;
}

void Ready::enter()
{
  log_.INFO("STM", "Entering Ready state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kReady;
  data_.setStateMachineData(sm_data);
}

void Ready::exit()
{
  log_.INFO("STM", "Exiting Ready state");
}

//--------------------------------------------------------------------------------------
//  Accelerating
//--------------------------------------------------------------------------------------

Accelerating *Accelerating::instance_;

State *Accelerating::checkTransition()
{
  updateModuleData();

  State *next;

  next = checkEmergencyMoving(log_, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                              sensors_data_, motors_data_);
  if (next) return next;

  next = checkEnteredBrakingZone(log_, nav_data_, telemetry_data_);
  if (next) return next;

  return NULL;
}

void Accelerating::enter()
{
  log_.INFO("STM", "Entering Accelerating state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kAccelerating;
  data_.setStateMachineData(sm_data);
}

void Accelerating::exit()
{
  log_.INFO("STM", "Exiting Accelerating state");
}

//--------------------------------------------------------------------------------------
//  Nominal Braking
//--------------------------------------------------------------------------------------

NominalBraking *NominalBraking::instance_;

State *NominalBraking::checkTransition()
{
  updateModuleData();

  State *next;

  next = checkEmergencyMoving(log_, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                              sensors_data_, motors_data_);
  if (next) return next;

  next = checkPodStoppedNominal(log_, nav_data_);
  if (next) return next;
  return NULL;
}

void NominalBraking::enter()
{
  log_.INFO("STM", "Entering NominalBraking state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kNominalBraking;
  data_.setStateMachineData(sm_data);
}

void NominalBraking::exit()
{
  log_.INFO("STM", "Exiting NominalBraking state");
}

//--------------------------------------------------------------------------------------
//  Finished
//--------------------------------------------------------------------------------------

Finished *Finished::instance_;

State *Finished::checkTransition()
{
  // We only need to update telemetry data.
  telemetry_data_ = data_.getTelemetryData();
  return checkShutdownCommand(log_, telemetry_data_);
}

void Finished::enter()
{
  log_.INFO("STM", "Entering Finished state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kFinished;
  data_.setStateMachineData(sm_data);
}

void Finished::exit()
{
  log_.INFO("STM", "Exiting Finished state");
}

//--------------------------------------------------------------------------------------
//  FailureBraking
//--------------------------------------------------------------------------------------

FailureBraking *FailureBraking::instance_;

State *FailureBraking::checkTransition()
{
  // We only need to update navigation data.
  nav_data_ = data_.getNavigationData();
  return checkPodStoppedEmergency(log_, nav_data_);
}

void FailureBraking::enter()
{
  log_.INFO("STM", "Entering FailureBraking state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kEmergencyBraking;
  data_.setStateMachineData(sm_data);
}

void FailureBraking::exit()
{
  log_.INFO("STM", "Exiting FailureBraking state");
}

//--------------------------------------------------------------------------------------
//  FailureStopped
//--------------------------------------------------------------------------------------

FailureStopped *FailureStopped::instance_;

State *FailureStopped::checkTransition()
{
  // We only need to update telemetry data.
  telemetry_data_ = data_.getTelemetryData();
  return checkShutdownCommand(log_, telemetry_data_);
}

void FailureStopped::enter()
{
  log_.INFO("STM", "Entering FailureStopped state");

  data::StateMachine sm_data = data_.getStateMachineData();
  sm_data.current_state      = data::State::kFailureStopped;
  data_.setStateMachineData(sm_data);
}

void FailureStopped::exit()
{
  log_.INFO("STM", "Exiting FailureStopped state");
}

//--------------------------------------------------------------------------------------
//  Off
//--------------------------------------------------------------------------------------

Off *Off::instance_;

State *Off::checkTransition()
{
  // This function should never be called.
  log_.ERR("STM", "Tried to transition from Off state");
  return NULL;
}

void Off::enter()
{
  utils::System &sys = utils::System::getSystem();
  log_.INFO("STM", "System is shutting down");
  sys.running_ = false;
}

void Off::exit()
{
  // We never exit this state anyways...
}

}  // namespace state_machine
}  // namespace hyped
