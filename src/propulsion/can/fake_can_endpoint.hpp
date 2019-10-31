/*
 * Author: Gregor Konzett
 * Organisation: HYPED
 * Date:
 * Description:
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

#ifndef PROPULSION_CAN_FAKE_CAN_ENDPOINT_HPP_
#define PROPULSION_CAN_FAKE_CAN_ENDPOINT_HPP_

#include "utils/concurrent/thread.hpp"
#include "utils/io/can.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::io::CanProccesor;
using utils::io::can::Frame;

namespace motor_control {

class FakeCanEndpoint : public Thread {
 public:
  explicit FakeCanEndpoint(CanProccesor *sender);
  void run() override;

 private:
  CanProccesor *sender_;
};
}}  // namespace hyped::motor_control

#endif  // PROPULSION_CAN_FAKE_CAN_ENDPOINT_HPP_
