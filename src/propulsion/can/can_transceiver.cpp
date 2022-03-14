#include "can_transceiver.hpp"

namespace hyped::propulsion {

CanTransceiver::CanTransceiver(const uint8_t node_id, IController &controller)
    : log_("CAN-TRANSCEIVER", utils::System::getSystem().config_.log_level_propulsion),
      node_id_(node_id),
      can_(utils::io::Can::getInstance()),
      sender_(can_),
      controller_(controller)
{
  can_.start();
}

bool CanTransceiver::sendMessage(utils::io::can::Frame &message)
{
  log_.info("Sending Message");
  bool messageSuccess = sender_.sendMessage(message);
  return messageSuccess;
}

void CanTransceiver::registerController()
{
  can_.registerProcessor(this);
}

void CanTransceiver::processNewData(utils::io::can::Frame &message)
{
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

bool CanTransceiver::hasId(uint32_t id, bool)
{
  for (uint32_t cobId : canIds) {
    if (cobId + node_id_ == id) { return true; }
  }
  return false;
}
}  // namespace hyped::propulsion
