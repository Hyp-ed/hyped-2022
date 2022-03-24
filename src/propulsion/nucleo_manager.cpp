#include "nucleo_manager.hpp"

#include <utils/system.hpp>

namespace hyped::propulsion {

NucleoManager::NucleoManager()
    : log_("NUCLEOMANAGER", utils::System::getSystem().config_.log_level_propulsion),
      sender_()
{
  can_frame_.id       = kNucleoTransmit;
  can_frame_.extended = false;
  can_frame_.len      = 4;
}

void NucleoManager::sendNucleoFrequency(uint32_t target_frequency)
{
  for (size_t i = 0; i < 4; ++i) {
    can_frame_.data[i] = target_frequency & 0xff;
    target_frequency >>= 8;
  }
  sender_.sendMessage(can_frame_);
}
}  // namespace hyped::propulsion
