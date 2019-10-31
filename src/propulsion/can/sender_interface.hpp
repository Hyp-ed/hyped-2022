/*
 * Author: Gregor Konzett
 * Organisation: HYPED
 * Date: 1.4.2019
 * Description: Handles the communication with the CAN Bus
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef PROPULSION_CAN_SENDER_INTERFACE_HPP_
#define PROPULSION_CAN_SENDER_INTERFACE_HPP_

#include "utils/io/can.hpp"

namespace hyped
{

namespace motor_control
{

// Types of CANopen messages, these are used for CAN ID's
constexpr uint32_t kEmgyTransmit = 0x80;
constexpr uint32_t kSdoReceive = 0x600;
constexpr uint32_t kSdoTransmit = 0x580;
constexpr uint32_t kNmtReceive = 0x000;
constexpr uint32_t kNmtTransmit = 0x700;
constexpr uint32_t kPdo1Transmit = 0x180;
constexpr uint32_t kPdo1Receive = 0x200;
constexpr uint32_t kPdo2Transmit = 0x280;
constexpr uint32_t kPdo2Receive = 0x300;
constexpr uint32_t kPdo3Transmit = 0x380;
constexpr uint32_t kPdo3Receive = 0x400;
constexpr uint32_t kPdo4Transmit = 0x480;
constexpr uint32_t kPdo4Receive = 0x500;

constexpr uint32_t canIds[13] {0x80, 0x600, 0x580, 0x000, 0x700,
                              0x180, 0x200, 0x280, 0x300, 0x380,
                              0x400, 0x480, 0x500};

class SenderInterface
{
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

}  // namespace motor_control
}  // namespace hyped

#endif  // PROPULSION_CAN_SENDER_INTERFACE_HPP_
