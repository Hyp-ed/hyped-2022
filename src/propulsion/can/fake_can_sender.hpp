/*
 * Author: Gregor Konzett
 * Organisation: HYPED
 * Date: 1.4.2019
 * Description: Implements a mock system for the CAN Bus communication
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

#ifndef PROPULSION_CAN_FAKE_CAN_SENDER_HPP_
#define PROPULSION_CAN_FAKE_CAN_SENDER_HPP_

#include <atomic>
#include <iostream>
#include "utils/io/can.hpp"
#include "utils/logger.hpp"
#include "sender_interface.hpp"
#include "propulsion/can/fake_can_endpoint.hpp"

namespace hyped
{
namespace motor_control
{
using utils::Logger;
using utils::io::Can;
using utils::io::CanProccesor;

class FakeCanSender : public CanProccesor, public SenderInterface
{
  public:
    FakeCanSender(Logger &log_, uint8_t id);

    bool sendMessage(utils::io::can::Frame &message) override;

    void registerController() override;

    void processNewData(utils::io::can::Frame &message) override;

    bool hasId(uint32_t id, bool extended) override;

    bool getIsSending() override;

  private:
    Logger log_;
    // Can& can_;
    std::atomic<bool> isSending;
    FakeCanEndpoint *endpoint;
};
}  // namespace motor_control
}  // namespace hyped

#endif  // PROPULSION_CAN_FAKE_CAN_SENDER_HPP_
