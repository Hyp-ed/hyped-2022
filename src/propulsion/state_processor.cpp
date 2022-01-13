#include "controller.hpp"
#include "fake_controller.hpp"
#include "state_processor.hpp"

namespace hyped::propulsion {

StateProcessor::StateProcessor(utils::Logger &log)
    : log_(log),
      sys_(utils::System::getSystem()),
      data_(data::Data::getInstance()),
      is_initialised_(false),
      has_critical_error_(false),
      rpm_regulator_()
{
  controllers_ = new ControllerInterface *[data::Motors::kNumMotors];
  if (sys_.fake_motors) {
    log_.INFO("Motor", "Intializing with fake controller");
    for (int i = 0; i < data::Motors::kNumMotors; i++) {
      controllers_[i] = new FakeController(log_, i, false);
    }
  } else {  // Use real controllers
    log_.INFO("Motor", "Intializing with real controller");
    for (int i = 0; i < data::Motors::kNumMotors; i++) {
      controllers_[i] = new Controller(log_, i);
    }
  }
}

void StateProcessor::initialiseMotors()
{
  registerControllers();
  configureControllers();
  log_.INFO("Motor", "Initialize Speed Calculator");
  bool error = false;
  if (rpm_regulator_.isFaulty()) {
    error               = true;
    has_critical_error_ = true;
    return;
  }
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    if (controllers_[i]->getFailure()) {
      error = true;
      break;
    }
  }
  if (!error) {
    is_initialised_ = true;
  } else {
    has_critical_error_ = true;
  }
}

void StateProcessor::sendOperationalCommand()
{
  prepareMotors();
}

void StateProcessor::registerControllers()
{
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    controllers_[i]->registerController();
  }
}

void StateProcessor::configureControllers()
{
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    controllers_[i]->configure();
  }
}

void StateProcessor::prepareMotors()
{
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    controllers_[i]->enterOperational();
  }
  previous_acceleration_time_ = 0;
}

void StateProcessor::enterPreOperational()
{
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    controllers_[i]->enterPreOperational();
  }
}

void StateProcessor::accelerate()
{
  if (is_initialised_) {
    auto motor_data = data_.getMotorData();
    for (int i = 0; i < data::Motors::kNumMotors; i++) {
      controllers_[i]->updateActualVelocity();
      motor_data.rpms[i] = controllers_[i]->getVelocity();
    }
    data_.setMotorData(motor_data);

    const auto now = utils::Timer::getTimeMicros();
    if (now - previous_acceleration_time_ > 5000) {
      log_.DBG3("Motor", "Accelerate");
      previous_acceleration_time_ = now;
      const auto velocity         = data_.getNavigationData().velocity;

      int32_t act_rpm     = calculateAverageRpm();
      int32_t act_current = calculateMaximumCurrent();
      int32_t act_temp    = calculateMaximumTemperature();

      int32_t rpm = rpm_regulator_.calculateRpm(velocity, act_rpm, act_current, act_temp);

      log_.INFO("MOTOR", "Sending %d rpm as target", rpm);

      for (int i = 0; i < data::Motors::kNumMotors; i++) {
        controllers_[i]->sendTargetVelocity(rpm);
      }
    }
  } else {
    log_.INFO("Motor", "State Processor not initialized");
  }
}

int32_t StateProcessor::calculateAverageRpm()
{
  int32_t total = 0;
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    controllers_[i]->updateActualVelocity();
    total += controllers_[i]->getVelocity();
  }
  return std::round(total / data::Motors::kNumMotors);
}

int16_t StateProcessor::calculateMaximumCurrent()
{
  const auto hp_packs = data_.getBatteriesData();
  int16_t max_current = 0;
  for (int i = 0; i < hp_packs.kNumHPBatteries; i++) {
    int16_t current = hp_packs.high_power_batteries[i].current;
    if (max_current < current) { max_current = current; }
  }
  return max_current;
}

int32_t StateProcessor::calculateMaximumTemperature()
{
  int32_t max_temp = 0;
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    controllers_[i]->updateMotorTemp();
    int32_t temp = controllers_[i]->getMotorTemp();
    if (max_temp < temp) { max_temp = temp; }
  }
  return max_temp;
}

void StateProcessor::quickStopAll()
{
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    controllers_[i]->quickStop();
  }
}

void StateProcessor::healthCheck()
{
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    controllers_[i]->healthCheck();
  }
}

bool StateProcessor::getFailure()
{
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    if (controllers_[i]->getFailure()) { return true; }
  }

  return false;
}

bool StateProcessor::isInitialised()
{
  return this->is_initialised_;
}

bool StateProcessor::hasCriticalFailure()
{
  return this->has_critical_error_;
}

}  // namespace hyped::propulsion
