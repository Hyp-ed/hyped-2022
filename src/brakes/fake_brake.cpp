#include "fake_brake.hpp"

namespace hyped::brakes {

FakeBrake::FakeBrake(const uint8_t id)
    : log_("FAKE-BRAKE", utils::System::getSystem().config_.log_level_brakes),
      data_(data::Data::getInstance()),
      brakes_data_(data_.getEmergencyBrakesData()),
      id_(id),
      fake_button_(false)
{
}

void FakeBrake::retract()
{
  if (!isEngaged()) { return; }
  log_.info("Sending a retract message to brake %u", id_);
  fake_button_ = true;
}

void FakeBrake::engage()
{
  if (isEngaged()) { return; }
  log_.info("Sending a engage message to brake %u", id_);
  fake_button_ = false;
}

bool FakeBrake::isEngaged()
{
  return !fake_button_;
}

}  // namespace hyped::brakes
