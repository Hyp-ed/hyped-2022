#include "fake_stepper.hpp"

namespace hyped::brakes {

FakeStepper::FakeStepper(utils::Logger &log, uint8_t id)
    : log_(log),
      data_(data::Data::getInstance()),
      brakes_data_(data_.getEmergencyBrakesData()),
      brake_id_(id),
      is_clamped_(true),
      fake_button_(false)
{
}

void FakeStepper::checkHome()
{
  if (fake_button_ && !brakes_data_.brakes_retracted[brake_id_ - 1]) {
    brakes_data_.brakes_retracted[brake_id_ - 1] = true;
    data_.setEmergencyBrakesData(brakes_data_);
  } else if (!fake_button_ && brakes_data_.brakes_retracted[brake_id_ - 1]) {
    brakes_data_.brakes_retracted[brake_id_ - 1] = false;
    data_.setEmergencyBrakesData(brakes_data_);
  }
}

void FakeStepper::sendRetract()
{
  log_.info("Sending a retract message to brake %i", brake_id_);
  fake_button_ = true;  // false = brakes are retracted
  is_clamped_  = false;
}

void FakeStepper::sendClamp()
{
  log_.info("Sending a engage message to brake %i", brake_id_);
  fake_button_ = false;  // false = brakes are clamped
  is_clamped_  = true;
}

void FakeStepper::checkAccFailure()
{
  if (!fake_button_) {  // false = brakes are clamped
    log_.error("Brake %b failure", brake_id_);
    brakes_data_.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setEmergencyBrakesData(brakes_data_);
  }
  return;
}

void FakeStepper::checkBrakingFailure()
{
  if (fake_button_) {  // true = brakes are retracted
    log_.error("Brake %b failure", brake_id_);
    brakes_data_.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setEmergencyBrakesData(brakes_data_);
  }
  return;
}

bool FakeStepper::checkClamped()
{
  return is_clamped_;
}
}  // namespace hyped::brakes
