#include "can_sender.hpp"

#include <utils/timer.hpp>

namespace hyped::propulsion {
  CanSender::CanSender(utils::Logger &log)
  :log_(log),
  can_(utils::io::Can::getInstance())
  {
    can_.start();
  }

bool CanSender::sendMessage(utils::io::can::Frame &message)
{
  log_.info("Sending Message");
  can_.send(message);
  is_sending_    = true;
  const auto now = utils::Timer::getTimeMicros();
  while (is_sending_) {
    if ((utils::Timer::getTimeMicros() - now) > kTimeout) {
      // TODO(Iain): Test the latency and set the TIMEOUT to a reasonable value.
      log_.error("Sender timeout reached");
      return false;
    }
  }
  return true;
}
}
