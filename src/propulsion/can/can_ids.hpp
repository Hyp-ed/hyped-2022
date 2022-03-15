#pragma once
#include <stdint.h>
namespace hyped {

namespace propulsion {

// Types of CANopen messages, these are used for CAN ID's
static constexpr uint32_t kEmgyTransmit   = 0x80;
static constexpr uint32_t kSdoReceive     = 0x600;
static constexpr uint32_t kSdoTransmit    = 0x580;
static constexpr uint32_t kNmtReceive     = 0x000;
static constexpr uint32_t kNmtTransmit    = 0x700;
static constexpr uint32_t kPdo1Transmit   = 0x180;
static constexpr uint32_t kPdo1Receive    = 0x200;
static constexpr uint32_t kPdo2Transmit   = 0x280;
static constexpr uint32_t kPdo2Receive    = 0x300;
static constexpr uint32_t kPdo3Transmit   = 0x380;
static constexpr uint32_t kPdo3Receive    = 0x400;
static constexpr uint32_t kPdo4Transmit   = 0x480;
static constexpr uint32_t kPdo4Receive    = 0x500;
static constexpr uint32_t kNucleoTransmit = 0x900;  // currently lowest priority, might change

static constexpr uint32_t canIds[14]{0x80,  0x600, 0x580, 0x000, 0x700, 0x180, 0x200,
                                     0x280, 0x300, 0x380, 0x400, 0x480, 0x500, 0x900};

}  // namespace propulsion
}  // namespace hyped
