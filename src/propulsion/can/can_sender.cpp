#include "can_sender.hpp"

namespace hyped {
namespace motor_control {
CanSender::CanSender(Logger &log, uint8_t node_id)
    : log_(log),
      node_id_(node_id),
      can_(Can::getInstance()),
      messageTimestamp(0)
{
  isSending = false;
  can_.start();
}

CanSender::CanSender(ControllerInterface *controller, uint8_t node_id, Logger &log)
    : log_(log),
      node_id_(node_id),
      can_(Can::getInstance()),
      controller_(controller),
      messageTimestamp(0)
{
  isSending = false;
  can_.start();
}

bool CanSender::sendMessage(utils::io::can::Frame &message)
{
  log_.INFO("MOTOR", "Sending Message");
  can_.send(message);
  isSending = true;

  timer.start();
  messageTimestamp = timer.getTimeMicros();

  while (isSending) {
    if ((timer.getTimeMicros() - messageTimestamp) > TIMEOUT) {
      // TODO(Iain): Test the latency and set the TIMEOUT to a reasonable value.
      log_.ERR("MOTOR", "Sender timeout reached");
      return false;
    }
  }

  return true;
}

void CanSender::registerController()
{
  can_.registerProcessor(this);
}

void CanSender::processNewData(utils::io::can::Frame &message)
{
  isSending = false;

  uint32_t id = message.id;
  if (id == kEmgyTransmit + node_id_) {
    controller_->processEmergencyMessage(message);
  } else if (id == kSdoTransmit + node_id_) {
    controller_->processSdoMessage(message);
  } else if (id == kNmtTransmit + node_id_) {
    controller_->processNmtMessage(message);
  } else {
    log_.ERR("MOTOR", "Controller %d: CAN message not recognised", node_id_);
  }
}

bool CanSender::hasId(uint32_t id, bool extended)
{
  for (uint32_t cobId : canIds) {
    if (cobId + node_id_ == id) { return true; }
  }

  return false;
}

bool CanSender::getIsSending()
{
  return isSending;
}
}  // namespace motor_control
}  // namespace hyped
