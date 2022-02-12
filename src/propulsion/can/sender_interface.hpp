#pragma once

#include <utils/io/can.hpp>

namespace hyped {

namespace propulsion {

// Types of CANopen messages, these are used for CAN ID's
constexpr uint32_t kEmgyTransmit = 0x80;
constexpr uint32_t kSdoReceive   = 0x600;
constexpr uint32_t kSdoTransmit  = 0x580;
constexpr uint32_t kNmtReceive   = 0x000;
constexpr uint32_t kNmtTransmit  = 0x700;
constexpr uint32_t kPdo1Transmit = 0x180;
constexpr uint32_t kPdo1Receive  = 0x200;
constexpr uint32_t kPdo2Transmit = 0x280;
constexpr uint32_t kPdo2Receive  = 0x300;
constexpr uint32_t kPdo3Transmit = 0x380;
constexpr uint32_t kPdo3Receive  = 0x400;
constexpr uint32_t kPdo4Transmit = 0x480;
constexpr uint32_t kPdo4Receive  = 0x500;
constexpr uint32_t kStnTransmit  = 0x900; //currently lowest priority, might change

constexpr uint32_t canIds[13]{0x80,  0x600, 0x580, 0x000, 0x700, 0x180, 0x200,
                              0x280, 0x300, 0x380, 0x400, 0x480, 0x500};

class ISender {
 public:
  /**
   * @brief { Sends CAN messages }
   */
  virtual bool sendMessage(utils::io::can::Frame &message) = 0;

  /**
   * @brief { Registers the controller to process incoming CAN messages}
   * */
  virtual void registerController() = 0;

  /**
   * @brief { Return if the can_sender is sending a CAN message right now }
   */
  virtual bool getIsSending() = 0;
};

}  // namespace propulsion
}  // namespace hyped
