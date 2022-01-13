#include "controller.hpp"
#include "fake_controller.hpp"
#include "state_processor.hpp"

namespace hyped::propulsion {

StateProcessor::StateProcessor(int motorAmount, utils::Logger &log)
    : log_(log),
      sys_(utils::System::getSystem()),
      data_(data::Data::getInstance()),
      motor_data_(data_.getMotorData()),
      motorAmount(motorAmount),
      initialized(false),
      criticalError(false),
      servicePropulsionSpeed(100),
      speed(0),
      regulator()
{
  useFakeController = sys_.fake_motors;
  navigationData    = data_.getNavigationData();
  controllers       = new ControllerInterface *[motorAmount];
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

void StateProcessor::initialiseMotors()
{
  registerControllers();
  configureControllers();
  log_.INFO("Motor", "Initialize Speed Calculator");
  bool error = false;
  if (regulator.isFaulty()) {
    error         = true;
    criticalError = true;
    return;
  }
  for (int i = 0; i < motorAmount; i++) {
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
  for (int i = 0; i < motorAmount; i++) {
    controllers[i]->registerController();
  }
}

void StateProcessor::configureControllers()
{
  for (int i = 0; i < motorAmount; i++) {
    controllers[i]->configure();
  }
}

void StateProcessor::prepareMotors()
{
  for (int i = 0; i < motorAmount; i++) {
    controllers[i]->enterOperational();
  }

  // Setup acceleration timer
  accelerationTimer.start();
  accelerationTimestamp = 0;
}

void StateProcessor::enterPreOperational()
{
  for (int i = 0; i < motorAmount; i++) {
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
      velocity              = navigationData.velocity;

      int32_t act_rpm     = calculateAverageRpm(controllers);
      int32_t act_current = calculateMaxCurrent();
      int32_t act_temp    = calculateMaxTemp(controllers);

      int32_t rpm = regulator.calculateRpm(velocity, act_rpm, act_current, act_temp);

      log_.INFO("MOTOR", "Sending %d rpm as target", rpm);

      for (int i = 0; i < motorAmount; i++) {
        controllers[i]->sendTargetVelocity(rpm);
      }
    }
  } else {
    log_.INFO("Motor", "State Processor not initialized");
  }
}

int32_t StateProcessor::calculateAverageRpm(ControllerInterface **controllers)
{
  int32_t total = 0;
  for (int i = 0; i < motorAmount; i++) {
    controllers[i]->updateActualVelocity();
    total += controllers[i]->getVelocity();
  }
  return std::round(total / motorAmount);
}

int16_t StateProcessor::calculateMaxCurrent()
{
  const auto hp_packs = data_.getBatteriesData();
  int16_t max_current = 0;
  for (int i = 0; i < hp_packs.kNumHPBatteries; i++) {
    int16_t current = hp_packs.high_power_batteries[i].current;
    if (max_current < current) { max_current = current; }
  }
  return max_current;
}

int32_t StateProcessor::calculateMaxTemp(ControllerInterface **controllers)
{
  int32_t max_temp = 0;
  for (int i = 0; i < motorAmount; i++) {
    controllers[i]->updateMotorTemp();
    int32_t temp = controllers[i]->getMotorTemp();
    if (max_temp < temp) { max_temp = temp; }
  }
  return max_temp;
}

void StateProcessor::quickStopAll()
{
  for (int i = 0; i < motorAmount; i++) {
    controllers[i]->quickStop();
  }
}

void StateProcessor::healthCheck()
{
  for (int i = 0; i < motorAmount; i++) {
    controllers[i]->healthCheck();
  }
}

bool StateProcessor::getFailure()
{
  for (int i = 0; i < motorAmount; i++) {
    if (controllers[i]->getFailure()) { return true; }
  }

  return false;
}

bool StateProcessor::isInitialized()
{
  return this->initialized;
}

bool StateProcessor::isCriticalFailure()
{
  return this->criticalError;
}

}  // namespace hyped::propulsion
