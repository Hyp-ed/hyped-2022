#include "nucleo_manager.hpp"

namespace hyped::propulsion {

NucleoManager::NucleoManager(utils::Logger &log) : log_(log), sender_(log, node_id_, *this)
{
  nucleo_message_.id       = kNucleoTransmit;
  nucleo_message_.extended = false;
  nucleo_message_.len      = 4;
}

void NucleoManager::sendNucleoFrequency(const uint32_t target_frequency)
{
  log_.info("Nucleo board: Setting frequency to &d", target_frequency);
  nucleo_message_.data[1] = target_frequency & 0xFF;
  nucleo_message_.data[2] = (target_frequency >> 8) & 0xFF;
  nucleo_message_.data[3] = (target_frequency >> 16) & 0xFF;
  nucleo_message_.data[4] = (target_frequency >> 24) & 0xFF;
  sender_.sendMessage(nucleo_message_);
}
}  // namespace hyped::propulsion
