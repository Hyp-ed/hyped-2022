#include "controller.hpp"
#include "fake_controller.hpp"
#include "state_processor.hpp"

namespace hyped::propulsion {

StateProcessor::StateProcessor(utils::Logger &log)
    : log_(log),
      sys_(utils::System::getSystem()),
      data_(data::Data::getInstance()),
      is_initialised_(false),
      rpm_regulator_()
{
  if (sys_.fake_motors) {
    log_.INFO("STATE-PROCESSOR", "constructing with fake controllers");
    for (size_t i = 0; i < data::Motors::kNumMotors; ++i) {
      controllers_.at(i) = std::make_unique<FakeController>(log_, i, false);
    }
  } else {  // Use real controllers
    log_.INFO("STATE-PROCESSOR", "constructing with real controllers");
    for (size_t i = 0; i < data::Motors::kNumMotors; ++i) {
      controllers_.at(i) = std::make_unique<Controller>(log_, i);
    }
  }
}

void StateProcessor::initialiseMotors()
{
  registerControllers();
  configureControllers();
  log_.INFO("STATE-PROCESSOR", "initialising");
  bool error = false;
  for (auto &controller : controllers_) {
    if (controller->getFailure()) {
      error = true;
      break;
    }
  }
  if (!error) { is_initialised_ = true; }
}

void StateProcessor::sendOperationalCommand()
{
  prepareMotors();
}

void StateProcessor::registerControllers()
{
  for (auto &controller : controllers_) {
    controller->registerController();
  }
}

void StateProcessor::configureControllers()
{
  for (auto &controller : controllers_) {
    controller->configure();
  }
}

void StateProcessor::prepareMotors()
{
  for (auto &controller : controllers_) {
    controller->enterOperational();
  }
  previous_acceleration_time_ = 0;
}

void StateProcessor::enterPreOperational()
{
  for (auto &controller : controllers_) {
    controller->enterPreOperational();
  }
}

void StateProcessor::accelerate()
{
  if (!is_initialised_) {
    log_.INFO("STATE-PROCESSOR", "not initialised");
    return;
  }

  auto motor_data = data_.getMotorData();
  for (size_t i = 0; i < data::Motors::kNumMotors; ++i) {
    controllers_.at(i)->updateActualVelocity();
    motor_data.rpms.at(i) = controllers_.at(i)->getVelocity();
  }
  data_.setMotorData(motor_data);

  const auto now = utils::Timer::getTimeMicros();
  if (now - previous_acceleration_time_ > 5000) {
    previous_acceleration_time_ = now;
    const auto velocity         = data_.getNavigationData().velocity;
    const auto act_rpm          = calculateAverageRpm();
    const auto rpm              = rpm_regulator_.calculateRpm(velocity, act_rpm);
    log_.INFO("STATE-PROCESSOR", "sending %d rpm as target", rpm);
    for (auto &controller : controllers_) {
      controller->sendTargetVelocity(rpm);
    }
  }
}

bool StateProcessor::isOverLimits()
{
  const auto act_temp    = calculateMaximumTemperature();
  const auto act_current = calculateMaximumCurrent();
  if (act_current > data::Motors::kMaximumCurrent) {
    log_.ERR("STATE-PROCESSOR", "motors over maximum current");
    return true;
  }
  if (act_temp > data::Motors::kMaximumTemperature) {
    log_.ERR("STATE-PROCESSOR", "motors overheating");
    return true;
  }
  return false;
}

int32_t StateProcessor::calculateAverageRpm()
{
  int32_t total = 0;
  for (auto &controller : controllers_) {
    controller->updateActualVelocity();
    total += controller->getVelocity();
  }
  // integer division should be good enough
  return total / data::Motors::kNumMotors;
}

int16_t StateProcessor::calculateMaximumCurrent()
{
  const auto hp_packs = data_.getBatteriesData();
  int16_t max_current = 0;
  for (size_t i = 0; i < hp_packs.kNumHPBatteries; ++i) {
    int16_t current = hp_packs.high_power_batteries.at(i).current;
    if (max_current < current) { max_current = current; }
  }
  return max_current;
}

int32_t StateProcessor::calculateMaximumTemperature()
{
  int32_t max_temp = 0;
  for (auto &controller : controllers_) {
    controller->updateMotorTemp();
    const auto temp = controller->getMotorTemp();
    if (max_temp < temp) { max_temp = temp; }
  }
  return max_temp;
}

void StateProcessor::quickStopAll()
{
  for (auto &controller : controllers_) {
    controller->quickStop();
  }
}

void StateProcessor::healthCheck()
{
  for (auto &controller : controllers_) {
    controller->healthCheck();
  }
}

bool StateProcessor::hasControllerFailure()
{
  for (auto &controller : controllers_) {
    if (controller->getFailure()) { return true; }
  }
  return false;
}

bool StateProcessor::isInitialised()
{
  return this->is_initialised_;
}

}  // namespace hyped::propulsion
