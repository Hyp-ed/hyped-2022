#include "fake_can_sender.hpp"

namespace hyped::propulsion {
FakeCanSender::FakeCanSender(utils::Logger &log_, uint8_t)
    : log_(log_),
      is_sending_(false),
      can_endpoint_(*this)
{
}

bool FakeCanSender::sendMessage(utils::io::can::Frame &)
{
  log_.info("sending");
  is_sending_ = true;
  can_endpoint_.start();
  while (is_sending_) {
    utils::concurrent::Thread::yield();
  }
  return true;
}

void FakeCanSender::registerController()
{
}

void FakeCanSender::processNewData(utils::io::can::Frame &)
{
  log_.info("processNewData");
  is_sending_ = false;
}

bool FakeCanSender::hasId(uint32_t, bool)
{
  return true;
}

bool FakeCanSender::getIsSending()
{
  return is_sending_;
}
}  // namespace hyped::propulsion
