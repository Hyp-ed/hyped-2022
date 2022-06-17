#include "can_receiver.hpp"

namespace hyped::propulsion {

CanReceiver::CanReceiver(const uint8_t node_id, IController &controller)
    : log_("CAN-TRANSCEIVER", utils::System::getSystem().config_.log_level_propulsion),
      node_id_(node_id),
      controller_(controller),
      can_(utils::io::Can::getInstance())
{
  can_.start();
}

void CanReceiver::registerController()
{
  can_.registerProcessor(this);
}

void CanReceiver::processNewData(utils::io::can::Frame &message)
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

bool CanReceiver::hasId(const uint32_t id, const bool extended)
{
  if (extended) { return false; }
  for (uint32_t cobId : kCanIds) {
    if (cobId + node_id_ == id) { return true; }
  }
  return false;
}
}  // namespace hyped::propulsion
