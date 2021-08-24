#include "stepper.hpp"

namespace hyped {
namespace brakes {

Stepper::Stepper(uint8_t enable_pin, uint8_t button_pin, Logger &log, uint8_t id)
    : log_(log),
      data_(data::Data::getInstance()),
      brakes_data_(data_.getEmergencyBrakesData()),
      command_pin_(enable_pin, utils::io::gpio::kOut, log_),
      button_(button_pin, utils::io::gpio::kIn, log_),
      brake_id_(id),
      is_clamped_(true)
{
  GPIO command_pin_(enable_pin, utils::io::gpio::kOut, log_);
  GPIO button_(button_pin, utils::io::gpio::kIn, log_);
}

void Stepper::checkHome()
{
  if (button_.read() && !brakes_data_.brakes_retracted[brake_id_ - 1]) {
    brakes_data_.brakes_retracted[brake_id_ - 1] = true;
    data_.setEmergencyBrakesData(brakes_data_);
  } else if (!button_.read() && brakes_data_.brakes_retracted[brake_id_ - 1]) {
    brakes_data_.brakes_retracted[brake_id_ - 1] = false;
    data_.setEmergencyBrakesData(brakes_data_);
  }
}

void Stepper::sendRetract()
{
  log_.INFO("Brakes", "Sending a retract message to brake %i", brake_id_);
  command_pin_.clear();
  is_clamped_ = false;
}

void Stepper::sendClamp()
{
  log_.INFO("Brakes", "Sending a engage message to brake %i", brake_id_);
  command_pin_.set();
  is_clamped_ = true;
}

void Stepper::checkAccFailure()
{
  if (!button_.read()) {  // false = brakes are clamped
    log_.ERR("Brakes", "Brake %b failure", brake_id_);
    brakes_data_.module_status = ModuleStatus::kCriticalFailure;
    data_.setEmergencyBrakesData(brakes_data_);
  }
}

void Stepper::checkBrakingFailure()
{
  if (button_.read()) {  // true = brakes are retracted
    log_.ERR("Brakes", "Brake %b failure", brake_id_);
    brakes_data_.module_status = ModuleStatus::kCriticalFailure;
    data_.setEmergencyBrakesData(brakes_data_);
  }
}

bool Stepper::checkClamped()
{
  return is_clamped_;
}

}  // namespace brakes
}  // namespace hyped
