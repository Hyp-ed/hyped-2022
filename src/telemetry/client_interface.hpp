/*
 * Authors: J. Ridley
 * Organisation: HYPED
 * Date: Dec 2019
 * Description:
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef TELEMETRY_CLIENT_INTERFACE_HPP_
#define TELEMETRY_CLIENT_INTERFACE_HPP_

#include <string>

namespace hyped {

using utils::Logger;

namespace telemetry {

class ClientInterface {
 public:
  /**
   * @brief Connect client to base station, ret
   * @return true if successful
   */
  virtual bool connect() = 0;
  /**
   * @brief Send message to base station
   * @param message String to send to base-station
   * @return true if successful
   */
  virtual bool sendData(std::string message) = 0;
  /**
   * @brief Receive message from base station
   * @return message as a string
   */
  virtual std::string receiveData() = 0;
};

}}  // namespace hyped::telemetry

#endif  // TELEMETRY_CLIENT_INTERFACE_HPP_
