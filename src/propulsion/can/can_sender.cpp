#include "can_sender.hpp"

namespace hyped::propulsion {
CanSender::CanSender(utils::Logger &log, utils::io::Can &can) : log_(log), can_(can)
{
  can_.start();
}

bool CanSender::sendMessage(utils::io::can::Frame &message)
{
  log_.info("Sending Message");
  bool sendSuccessful = can_.send(message);
  return sendSuccessful;
}
}  // namespace hyped::propulsion
