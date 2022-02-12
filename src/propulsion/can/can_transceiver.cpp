#include "can_transceiver.hpp"

namespace hyped::propulsion {

CanSender::CanSender(utils::Logger &log, const uint8_t node_id, IController &controller)
    : log_(log),
      node_id_(node_id),
      can_(utils::io::Can::getInstance()),
      sender_(propulsion::can::can_sender::Sender(log_)),
      controller_(controller)
{
  is_sending_ = false;
  can_.start();
}

void /*bool*/ CanSender::sendMessage(utils::io::can::Frame &message)
{
  log_.info("Sending Message");
  sender_.send(&message);
}

void CanSender::registerController()
{
  can_.registerProcessor(this);
}

void CanSender::processNewData(utils::io::can::Frame &message)
{
  is_sending_ = false;
  uint32_t id = message.id;
  if (id == kEmgyTransmit + node_id_) {
    controller_.processEmergencyMessage(message);
  } else if (id == kSdoTransmit + node_id_) {
    controller_.processSdoMessage(message);
  } else if (id == kNmtTransmit + node_id_) {
    controller_.processNmtMessage(message);
  } else {
    log_.error("Controller %d: CAN message not recognised", node_id_);
  }
}

bool CanSender::hasId(uint32_t id, bool)
{
  for (uint32_t cobId : canIds) {
    if (cobId + node_id_ == id) { return true; }
  }
  return false;
}

bool CanSender::getIsSending()
{
  return is_sending_;
}
}  // namespace hyped::propulsion
