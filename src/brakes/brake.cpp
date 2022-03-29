#include "brake.hpp"

namespace hyped::brakes {

Brake::Brake(const uint8_t enable_pin, const uint8_t button_pin, const uint8_t id)
    : log_("BRAKE", utils::System::getSystem().config_.log_level_brakes),
      command_pin_(enable_pin, utils::io::Gpio::Direction::kOut, log_),
      button_(button_pin, utils::io::Gpio::Direction::kIn, log_),
      id_(id)
{
  utils::io::Gpio command_pin_(enable_pin, utils::io::Gpio::Direction::kOut, log_);
  utils::io::Gpio button_(button_pin, utils::io::Gpio::Direction::kIn, log_);
}

void Brake::retract()
{
  if (!isEngaged()) { return; }
  log_.info("Sending a retract message to brake %u", id_);
  command_pin_.clear();
}

void Brake::engage()
{
  if (isEngaged()) { return; }
  log_.info("Sending a engage message to brake %u", id_);
  command_pin_.set();
}

bool Brake::isEngaged()
{
  return !button_.read();
}

}  // namespace hyped::brakes
