#pragma once
#include <stdint.h>

#include <array>
namespace hyped {

namespace propulsion {

// Types of CANopen messages, these are used for CAN ID's
static constexpr uint32_t kNmtReceive     = 0x000;
static constexpr uint32_t kEmgyTransmit   = 0x80;
static constexpr uint32_t kPdo1Transmit   = 0x180;
static constexpr uint32_t kPdo1Receive    = 0x200;
static constexpr uint32_t kPdo2Transmit   = 0x280;
static constexpr uint32_t kPdo2Receive    = 0x300;
static constexpr uint32_t kPdo3Transmit   = 0x380;
static constexpr uint32_t kPdo3Receive    = 0x400;
static constexpr uint32_t kPdo4Transmit   = 0x480;
static constexpr uint32_t kPdo4Receive    = 0x500;
static constexpr uint32_t kSdoTransmit    = 0x580;
static constexpr uint32_t kSdoReceive     = 0x600;
static constexpr uint32_t kNmtTransmit    = 0x700;
static constexpr uint32_t kNucleoTransmit = 0x900;  // currently lowest priority, might change

static constexpr std::array<uint32_t, 14> kCanIds
  = {kEmgyTransmit, kSdoReceive,   kSdoTransmit,  kNmtReceive,    kNmtTransmit,
     kPdo1Transmit, kPdo1Receive,  kPdo2Transmit, kPdo2Receive,   kPdo3Transmit,
     kPdo3Receive,  kPdo4Transmit, kPdo4Receive,  kNucleoTransmit};

}  // namespace propulsion
}  // namespace hyped
