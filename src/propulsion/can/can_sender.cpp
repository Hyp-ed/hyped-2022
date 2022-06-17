#include "can_sender.hpp"

#include <utils/system.hpp>

namespace hyped::propulsion {
CanSender::CanSender()
    : log_("CAN-SENDER", utils::System::getSystem().config_.log_level_propulsion),
      can_(utils::io::Can::getInstance())
{
  can_.start();
}

bool CanSender::sendMessage(const utils::io::can::Frame &message)
{
  log_.info("Sending Message");
  return can_.send(message);
}
}  // namespace hyped::propulsion
