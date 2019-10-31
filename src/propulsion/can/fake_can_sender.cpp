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

#include "propulsion/can/fake_can_sender.hpp"

namespace hyped
{
namespace motor_control
{
FakeCanSender::FakeCanSender(Logger &log_, uint8_t node_id) : log_(log_)
{
  isSending = false;
  endpoint = new FakeCanEndpoint(this);
}

bool FakeCanSender::sendMessage(utils::io::can::Frame &message)
{
  log_.INFO("MOTOR", "sending");

  isSending = true;
  endpoint->start();

  while (isSending);

  return true;
}

void FakeCanSender::registerController()
{
}

void FakeCanSender::processNewData(utils::io::can::Frame &message)
{
  log_.INFO("MOTOR", "processNewData");
  isSending = false;
}

bool FakeCanSender::hasId(uint32_t id, bool extended)
{
  return true;
}

bool FakeCanSender::getIsSending()
{
  return isSending;
}
}  // namespace motor_control
}  // namespace hyped
