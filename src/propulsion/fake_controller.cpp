#include "fake_controller.hpp"

namespace hyped::propulsion {

FakeController::FakeController(utils::Logger &log, const uint8_t id, const bool is_faulty)
    : log_(log),
      data_(data::Data::getInstance()),
      id_(id),
      is_faulty_(is_faulty),
      critical_failure_(false),
      actual_velocity_(0),
      start_time_(0),
      timer_started_(false),
      motor_temperature_(60)
{
}

void FakeController::registerController()
{ /*EMPTY*/
}

void FakeController::configure()
{
  log_.info("Controller %d: Configuring", id_);
}

void FakeController::startTimer()
{
  start_time_    = utils::Timer::getTimeMicros();
  timer_started_ = true;
  fail_time_     = std::rand() % 20000000 + 1000000;
}

void FakeController::enterOperational()
{
  state_ = kOperationEnabled;
  log_.debug("Controller %d: entering operational", id_);
}

void FakeController::enterPreOperational()
{
  if (state_ != kSwitchOnDisabled) { log_.debug("Controller %d: shutting down motor", id_); }
  state_           = kSwitchOnDisabled;
  actual_velocity_ = 0;
}

void FakeController::checkState()
{
  log_.debug("Controller %d: checking status", id_);
}

void FakeController::sendTargetVelocity(const int32_t target_velocity)
{
  if (!timer_started_) { startTimer(); }
  log_.debug("Controller %d: updating target velocity to %d", id_, target_velocity);
  actual_velocity_ = target_velocity;
}

void FakeController::updateActualVelocity()
{ /*EMPTY*/
}

int32_t FakeController::getVelocity()
{
  return actual_velocity_;
}

void FakeController::quickStop()
{
  log_.debug("Controller %d: sending quick stop command", id_);
}

void FakeController::healthCheck()
{
  if (!is_faulty_) { return; }
  const data::State state = data_.getStateMachineData().current_state;
  if (state != data::State::kAccelerating && state != data::State::kCruising
      && state != data::State::kNominalBraking) {
    return;
  }
  if (fail_time_ <= (timer_.getMicros() - start_time_)) {
    critical_failure_ = true;
    log_.error("fake critical failure");
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
  return motor_temperature_;
}
}  // namespace hyped::propulsion
