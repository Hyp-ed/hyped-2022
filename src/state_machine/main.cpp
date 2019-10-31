/*
 * Author:Ragnor Comerford and Kornelija Sukyte
 * Organisation: HYPED
 * Date:
 * Description:
 *
 *    Copyright 2019 HYPED
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
#include <cstdint>

#include "state_machine/hyped-machine.hpp"
#include "state_machine/main.hpp"

#include "data/data.hpp"
#include "utils/timer.hpp"
#include "utils/system.hpp"
#include "utils/config.hpp"

namespace hyped {
namespace state_machine {

Main::Main(uint8_t id, Logger& log)
    : Thread(id, log),
      hypedMachine(log),
      sys_(utils::System::getSystem()),
      data_(data::Data::getInstance())
{
  timeout_ = static_cast<uint64_t>(sys_.config->statemachine.timeout);
}

/**
  *  @brief  Runs state machine thread.
  */

void Main::run()
{
  utils::System& sys = utils::System::getSystem();

  while (sys.running_) {
    telemetry_data_ = data_.getTelemetryData();
    nav_data_       = data_.getNavigationData();
    sm_data_        = data_.getStateMachineData();
    motor_data_     = data_.getMotorData();
    batteries_data_ = data_.getBatteriesData();
    sensors_data_   = data_.getSensorsData();
    emergency_brakes_data_  = data_.getEmergencyBrakesData();

    switch (sm_data_.current_state) {
      case data::State::kIdle:
        if (checkTelemetryCriticalFailure()) break;
        if (checkInitialised())          break;
        break;
      case data::State::kCalibrating:
        if (checkCriticalFailure())      break;
        if (checkSystemsChecked())       break;
        break;
      case data::State::kReady:
        if (checkCriticalFailure())      break;
        if (checkOnStart())              break;
        break;
      case data::State::kAccelerating:
        if (checkCriticalFailure())      break;
        if (checkTimer())                break;
        if (checkMaxDistanceReached())   break;
        break;
      case data::State::kNominalBraking:
        if (checkCriticalFailure())      break;
        if (checkAtRest())  break;
        break;
      case data::State::kRunComplete:
        if (checkCriticalFailure())      break;
        if (checkOnExit())               break;
        break;
      case data::State::kExiting:
        if (checkCriticalFailure())      break;
        if (checkFinish())               break;
        break;
      case data::State::kEmergencyBraking:
        if (checkAtRest())  break;
        break;
      // we cannot recover from these states
      case data::State::kInvalid:
        log_.ERR("STATE", "we are in Invalid state");
      case data::State::kFinished:
        if (checkReset())                break;
      case data::State::kFailureStopped:
      default:
        break;
    }

    yield();
  }
}

bool Main::checkInitialised()
{
  // all modules must be initialised
  if (telemetry_data_.module_status == ModuleStatus::kInit &&
      nav_data_.module_status       == ModuleStatus::kInit &&
      motor_data_.module_status     == ModuleStatus::kInit &&
      sensors_data_.module_status   == ModuleStatus::kInit &&
      batteries_data_.module_status == ModuleStatus::kInit &&
      telemetry_data_.calibrate_command) {
    log_.INFO("STATE", "all modules are initialised and Start Calibrating command was received");
    hypedMachine.handleEvent(kInitialised);
    telemetry_data_.calibrate_command = false;
    data_.setTelemetryData(telemetry_data_);
    return true;
  }
  return false;
}

bool Main::checkSystemsChecked()
{
  // nav and motors must be ready
  if (nav_data_.module_status   == ModuleStatus::kReady &&
      motor_data_.module_status == ModuleStatus::kReady &&
      emergency_brakes_data_.module_status == ModuleStatus::kReady) {
    log_.INFO("STATE", "systems ready");
    hypedMachine.handleEvent(kSystemsChecked);
    return true;
  }
  return false;
}

bool Main::checkReset()
{
  if (telemetry_data_.reset_command) {
    log_.INFO("STATE", "reset command received");
    hypedMachine.handleEvent(kReset);
    telemetry_data_.reset_command = false;  // reset the command to false
    data_.setTelemetryData(telemetry_data_);
    return true;
  }
  return false;
}

bool Main::checkOnStart()
{
  if (telemetry_data_.launch_command) {
    log_.INFO("STATE", "launch command received");
    hypedMachine.handleEvent(kOnStart);
    telemetry_data_.launch_command = false;
    data_.setTelemetryData(telemetry_data_);
    // also setup timer for going to emergency braking state
    time_start_ = utils::Timer::getTimeMicros();
    return true;
  }
  return false;
}

bool Main::checkTelemetryCriticalFailure()
{
  if (telemetry_data_.module_status == ModuleStatus::kCriticalFailure) {
    log_.ERR("STATE", "Critical failure caused by telemetry ");
    hypedMachine.handleEvent(kCriticalFailure);
    return true;
  }
  // Also check if emergency stop command was received
  if (telemetry_data_.emergency_stop_command) {
    log_.ERR("STATE", "STOP command received");
    telemetry_data_.emergency_stop_command = false;
    data_.setTelemetryData(telemetry_data_);
    hypedMachine.handleEvent(kCriticalFailure);
    return true;
  }
  return false;
}

bool Main::checkCriticalFailure()
{
  bool criticalFailureFound = false;
  // check if any of the module has failed (except sensors)
  if (telemetry_data_.module_status == ModuleStatus::kCriticalFailure) {
    log_.ERR("STATE", "Critical failure caused by telemetry ");
    criticalFailureFound = true;
    // return true
  }
  if (nav_data_.module_status == ModuleStatus::kCriticalFailure) {
    log_.ERR("STATE", "Critical failure caused by navigation ");
    criticalFailureFound = true;
    // return true;
  }
  if (motor_data_.module_status == ModuleStatus::kCriticalFailure) {
    log_.ERR("STATE", "Critical failure caused by motors ");
    criticalFailureFound = true;
    // return true;
  }
  if (batteries_data_.module_status == ModuleStatus::kCriticalFailure) {
    log_.ERR("STATE", "Critical failure caused by batteries ");
    criticalFailureFound = true;
    // return true;
  }
  if (emergency_brakes_data_.module_status == ModuleStatus::kCriticalFailure) {
    log_.ERR("STATE", "Critical failure caused by emergency brakes ");
    criticalFailureFound = true;
    // return true;
  }
  // Also check if emergency stop command was received
  if (telemetry_data_.emergency_stop_command) {
    log_.ERR("STATE", "STOP command received");
    telemetry_data_.emergency_stop_command = false;
    data_.setTelemetryData(telemetry_data_);
    criticalFailureFound = true;
    // return true;
  }
  if (criticalFailureFound) {
    hypedMachine.handleEvent(kCriticalFailure);
    return true;
  }
  return false;
}

bool Main::checkMaxDistanceReached()
{
  if (nav_data_.distance +
      nav_data_.braking_distance +
      20 >= data::Telemetry::run_length) {
    log_.INFO("STATE", "max distance reached");
    log_.INFO("STATE", "current distance, braking distance: %f %f"
      , nav_data_.distance
      , nav_data_.braking_distance);
    hypedMachine.handleEvent(kMaxDistanceReached);
    return true;
  }
  return false;
}

bool Main::checkOnExit()
{
  if (telemetry_data_.service_propulsion_go) {
    log_.INFO("STATE", "initialising service propulsion");
    hypedMachine.handleEvent(kOnExit);
    return true;
  }
  return false;
}

bool Main::checkFinish()
{
  // Check if end of tube was reached (leniency of 20m)
  if (nav_data_.distance + 20 >= data::Telemetry::run_length) {
        log_.INFO("STATE", "ready for collection");
        hypedMachine.handleEvent(kFinish);
        return true;
      }
  return false ;
}

bool Main::checkAtRest()
{
  if (nav_data_.acceleration >= -0.1 && nav_data_.acceleration <= 0.1 &&
      !emergency_brakes_data_.brakes_retracted[0] && !emergency_brakes_data_.brakes_retracted[1] &&
      !emergency_brakes_data_.brakes_retracted[2] && !emergency_brakes_data_.brakes_retracted[3]) {
    log_.INFO("STATE", "RPM reached zero.");
    hypedMachine.handleEvent(kAtRest);
    return true;
  }
  return false;
}

bool Main::checkTimer()
{
  if (utils::Timer::getTimeMicros() > time_start_ + timeout_) {
    log_.ERR("STATE", "Timer expired");
    hypedMachine.handleEvent(kCriticalFailure);
    return true;
  }
  return false;
}

}}  // namespace hyped::state_machine
