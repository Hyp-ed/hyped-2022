#include "fake_can_receiver.hpp"

namespace hyped::propulsion {
FakeCanReceiver::FakeCanReceiver(uint8_t)
    : log_("FAKE-CAN-SENDER", utils::System::getSystem().config_.log_level_propulsion),
      is_sending_(false),
      can_endpoint_(*this)
{
}

void FakeCanReceiver::registerController()
{
}

void FakeCanReceiver::processNewData(utils::io::can::Frame &)
{
  log_.info("processNewData");
  is_sending_ = false;
}

bool FakeCanReceiver::hasId(const uint32_t, bool)
{
  return true;
}

bool FakeCanReceiver::getIsSending()
{
  return is_sending_;
}
}  // namespace hyped::propulsion
