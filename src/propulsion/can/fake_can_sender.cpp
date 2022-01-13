#include "fake_can_sender.hpp"

namespace hyped {
namespace propulsion {
FakeCanSender::FakeCanSender(Logger &log_, uint8_t) : log_(log_)
{
  isSending = false;
  endpoint  = new FakeCanEndpoint(this);
}

bool FakeCanSender::sendMessage(utils::io::can::Frame &)
{
  log_.INFO("MOTOR", "sending");

  isSending = true;
  endpoint->start();

  while (isSending)
    ;

  return true;
}

void FakeCanSender::registerController()
{
}

void FakeCanSender::processNewData(utils::io::can::Frame &)
{
  log_.INFO("MOTOR", "processNewData");
  isSending = false;
}

bool FakeCanSender::hasId(uint32_t, bool)
{
  return true;
}

bool FakeCanSender::getIsSending()
{
  return isSending;
}
}  // namespace propulsion
}  // namespace hyped
