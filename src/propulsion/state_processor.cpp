/*
* Author: Gregor Konzett
* Organisation: HYPED
* Date: 1.4.2019
* Description: Handles the different states of the state machine
*
*    Copyright 2019 HYPED
*    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
*    except in compliance with the License. You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
*    Unless required by applicable law or agreed to in writing, software distributed under
*    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
*    either express or implied. See the License for the specific language governing permissions and
*    limitations under the License.
*/

#include "propulsion/state_processor.hpp"

namespace hyped
{

namespace motor_control
{

StateProcessor::StateProcessor(int motorAmount, Logger &log)
  : log_(log),
  sys_(System::getSystem()),
  data_(Data::getInstance()),
  motor_data_(data_.getMotorData()),
  motorAmount(motorAmount),
  initialized(false),
  criticalError(false),
  servicePropulsionSpeed(100),
  speed(0),
  regulator(log_)
{
  // rpmCalculator = new CalculateRPM(log);

  useFakeController = sys_.fake_motors;

  navigationData = data_.getNavigationData();

  controllers = new ControllerInterface*[motorAmount];

  if (useFakeController) {  // Use the test controllers implementation
    log_.INFO("Motor", "Intializing with fake controller");
    for (int i = 0; i < motorAmount; i++) {
      controllers[i] = new FakeController(log_, i, false);
    }
  } else {  // Use real controllers
    log_.INFO("Motor", "Intializing with real controller");
    for (int i = 0; i < motorAmount; i++) {
      controllers[i] = new Controller(log_, i);
    }
  }
}

void StateProcessor::initMotors()
{
  // Register controllers on CAN bus
  registerControllers();

  // Configure controllers parameters
  configureControllers();

  log_.INFO("Motor", "Initialize Speed Calculator");

  bool error = false;

  if (regulator.getFailure()) {
    error = true;
    criticalError = true;
    return;
  }

  for (int i = 0;i < motorAmount;i++) {
    if (controllers[i]->getFailure()) {
      error = true;
      break;
    }
  }
  if (!error) {
    initialized = true;
  } else {
    criticalError = true;
  }
}

  void StateProcessor::sendOperationalCommand()
  {
    prepareMotors();
  }

void StateProcessor::registerControllers()
{
  for (int i = 0;i < motorAmount; i++) {
    controllers[i]->registerController();
  }
}

void StateProcessor::configureControllers()
{
  for (int i = 0;i < motorAmount; i++) {
    controllers[i]->configure();
  }
}

void StateProcessor::prepareMotors()
{
  for (int i = 0;i < motorAmount; i++) {
    controllers[i]->enterOperational();
  }

  // Setup acceleration timer
  accelerationTimer.start();
  accelerationTimestamp = 0;
}

void StateProcessor::enterPreOperational()
{
  for (int i = 0;i < motorAmount; i++) {
    controllers[i]->enterPreOperational();
  }
}

void StateProcessor::accelerate()
{
  if (initialized) {
    motor_data_ = data_.getMotorData();
    for (int i = 0; i < motorAmount; i++) {
      controllers[i]->updateActualVelocity();
      motor_data_.rpms[i] = controllers[i]->getVelocity();
    }
    data_.setMotorData(motor_data_);

    if (accelerationTimer.getTimeMicros() - accelerationTimestamp > 5000) {
      log_.DBG3("Motor", "Accelerate");
      accelerationTimestamp = accelerationTimer.getTimeMicros();
      velocity = navigationData.velocity;

      int32_t act_rpm = calcAverageRPM(controllers);
      int32_t act_current = calcMaxCurrent();
      int32_t act_temp = calcMaxTemp(controllers);

      int32_t rpm = regulator.calculateRPM(velocity, act_rpm, act_current, act_temp);

      log_.INFO("MOTOR", "Sending %d rpm as target", rpm);

      for (int i = 0;i < motorAmount; i++) {
        controllers[i]->sendTargetVelocity(rpm);
      }
    }
  } else {
    log_.INFO("Motor", "State Processor not initialized");
  }
}

int32_t StateProcessor::calcAverageRPM(ControllerInterface** controllers)
{
  int32_t total = 0;
  for (int i = 0; i < motorAmount; i++) {
    controllers[i]->updateActualVelocity();
    total += controllers[i]->getVelocity();
  }
  return std::round(total/motorAmount);
}

int16_t StateProcessor::calcMaxCurrent()
{
  Batteries hp_packs = data_.getBatteriesData();
  int16_t max_current = 0;
  for (int i = 0; i < hp_packs.kNumHPBatteries; i++) {
    int16_t current = hp_packs.high_power_batteries[i].current;
    if (max_current < current) {
      max_current = current;
    }
  }
  return max_current;
}

int32_t StateProcessor::calcMaxTemp(ControllerInterface** controllers)
{
  int32_t max_temp = 0;
  for (int i = 0; i < motorAmount; i++) {
    controllers[i]->updateMotorTemp();
    int32_t temp = controllers[i]->getMotorTemp();
    if (max_temp < temp) {
      max_temp = temp;
    }
  }
  return max_temp;
}

void StateProcessor::quickStopAll()
{
  for (int i = 0;i < motorAmount; i++) {
    controllers[i]->quickStop();
  }
}

void StateProcessor::healthCheck()
{
  for (int i = 0;i < motorAmount; i++) {
    controllers[i]->healthCheck();
  }
}

bool StateProcessor::getFailure()
{
  for (int i = 0;i < motorAmount; i++) {
    if (controllers[i]->getFailure()) {
      return true;
    }
  }

  return false;
}

void StateProcessor::servicePropulsion()
{
  for (int i = 0;i < motorAmount; i++) {
    controllers[i]->sendTargetVelocity(servicePropulsionSpeed);
  }
}

bool StateProcessor::isInitialized()
{
  return this->initialized;
}

bool StateProcessor::isCriticalFailure()
{
  return this->criticalError;
}

}}  // hyped::motor_control
