#include "can_sender.hpp"

namespace hyped::propulsion {
CanSender::CanSender(utils::io::Can &can)
    : log_("CAN-SENDER", utils::System::getSystem().config_.log_level_propulsion),
      can_(can)
{
  can_.start();
}

bool CanSender::sendMessage(utils::io::can::Frame &message)
{
  log_.info("Sending Message");
  return can_.send(message);
}
}  // namespace hyped::propulsion
