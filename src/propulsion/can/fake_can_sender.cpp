#include "fake_can_sender.hpp"

namespace hyped::propulsion {
FakeCanSender::FakeCanSender(uint8_t)
    : log_("FAKE-CAN-SENDER", utils::System::getSystem().config_.log_level_propulsion),
      is_sending_(false),
      can_endpoint_(*this)
{
}

bool FakeCanSender::sendMessage(const utils::io::can::Frame &)
{
  log_.info("sending");
  is_sending_ = true;
  can_endpoint_.start();
  while (is_sending_) {
    utils::concurrent::Thread::yield();
  }
  return true;
}

bool FakeCanSender::getIsSending()
{
  return is_sending_;
}
}  // namespace hyped::propulsion
