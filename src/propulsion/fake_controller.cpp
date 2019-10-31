/*
 * Author: Iain Macpherson
 * Organisation: HYPED
 * Date: 11/03/2019
 * Description: Main class for fake Controllers
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
#include "propulsion/fake_controller.hpp"

namespace hyped {
namespace motor_control {

FakeController::FakeController(Logger& log, uint8_t id, bool isFaulty)
  : log_(log),
    data_(data::Data::getInstance()),
    motor_data_(data_.getMotorData()),
    id_(id),
    isFaulty_(isFaulty),
    critical_failure_(false),
    actual_velocity_(0),
    start_time_(0),
    timer_started_(false),
    motor_temp_(60)
{
}

void FakeController::registerController()
{/*EMPTY*/}

void FakeController::configure()
{
  log_.INFO("MOTOR", "Controller %d: Configuring", id_);
}

void FakeController::startTimer()
{
  start_time_ = Timer::getTimeMicros();
  timer_started_ = true;
  fail_time_ = std::rand() % 20000000 + 1000000;
}

void FakeController::enterOperational()
{
  state_ = kOperationEnabled;
  log_.DBG1("MOTOR", "Controller %d: Entering Operational", id_);
}

void FakeController::enterPreOperational()
{
  if (state_ != kSwitchOnDisabled) {
    log_.DBG1("MOTOR", "Controller %d: Shutting down motor", id_);
  }
  state_ = kSwitchOnDisabled;
  actual_velocity_ = 0;
}

void FakeController::checkState()
{
  log_.DBG1("MOTOR", "Controller %d: Checking status", id_);
}

void FakeController::sendTargetVelocity(int32_t target_velocity)
{
  if (!timer_started_) {
    startTimer();
  }
  log_.DBG2("MOTOR", "Controller %d: Updating target velocity to %d", id_, target_velocity);
  actual_velocity_ = target_velocity;
}

void FakeController::updateActualVelocity()
{/*EMPTY*/}

int32_t FakeController::getVelocity()
{
  return actual_velocity_;
}

void FakeController::quickStop()
{
  log_.DBG1("MOTOR", "Controller %d: Sending quick stop command", id_);
}

void FakeController::healthCheck()
{
  if (isFaulty_) {
    data::State state = data_.getStateMachineData().current_state;
    if (state == data::State::kAccelerating || state == data::State::kNominalBraking) {
      if (fail_time_ <= (timer.getMicros() - start_time_)) {
        critical_failure_ = true;
        log_.ERR("FakeController", "Fake critical failure");
      }
    }
  }
}

bool FakeController::getFailure()
{
  return critical_failure_;
}

ControllerState FakeController::getControllerState()
{
  return state_;
}

uint8_t FakeController::getMotorTemp()
{
  return motor_temp_;
}
}}  // namespace hyped::motor_control
