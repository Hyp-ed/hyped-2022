#include "can_listener.hpp"

#include <utils/system.hpp>

namespace hyped::debugging {

CanListener::CanListener() : log_("CAN-LISTENER", utils::System::getSystem().config_.log_level)
{
}

void CanListener::processNewData(utils::io::can::Frame &message)
{
  log_.info("Received CAN frame: {node_id=%u, extended=%s, len=%u, data=%x %x %x %x %x %x %x %x}",
            message.id, message.id, message.len, message.data[0], message.data[1], message.data[2],
            message.data[3], message.data[4], message.data[5], message.data[6], message.data[7]);
}

bool CanListener::hasId(uint32_t id, bool extended)
{
  if (extended) {
    log_.debug("received extended CAN message; skipping");
    return false;
  }
  return ids_.find(id) != ids_.end(); 
}

void CanListener::subscribe(const uint32_t id)
{
  ids_.insert(id);
}

void CanListener::unsubscribe(const uint32_t id)
{
  if (ids_.find(id) == ids_.end()) {
    log_.error("tried to unsubscribe from unknown node_id");
    return;
  }
  ids_.erase(id);
}

}  // namespace hyped::debugging
