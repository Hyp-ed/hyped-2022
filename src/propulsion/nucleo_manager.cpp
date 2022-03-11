#include "nucleo_manager.hpp"

namespace hyped::propulsion {

NucleoManager::NucleoManager() : log_("NUCLEOMANAGER"), sender_(log_, utils::io::Can::getInstance())
{
  can_frame_.id       = kNucleoTransmit;
  can_frame_.extended = false;
  can_frame_.len      = 4;
}

void NucleoManager::sendNucleoFrequency(const uint32_t target_frequency)
{
  can_frame_.data[1] = target_frequency & 0xFF;
  can_frame_.data[2] = (target_frequency >> 8) & 0xFF;
  can_frame_.data[3] = (target_frequency >> 16) & 0xFF;
  can_frame_.data[4] = (target_frequency >> 24) & 0xFF;
  sender_.sendMessage(can_frame_);
}
}  // namespace hyped::propulsion
