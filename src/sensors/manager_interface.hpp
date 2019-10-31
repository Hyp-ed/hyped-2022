/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 21/06/18
 * Description: Main interface for the manager classes.
 *
 *    Copyright 2018 HYPED
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

#ifndef SENSORS_MANAGER_INTERFACE_HPP_
#define SENSORS_MANAGER_INTERFACE_HPP_

#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped {

using data::ImuData;
using data::BatteryData;
using utils::concurrent::Thread;
using data::NavigationVector;

namespace sensors {

class ManagerInterface : public Thread {
 public:
  /**
   * @brief Checks if the data has been updated
   *
   */
  explicit ManagerInterface(utils::Logger& log) : Thread(log), old_timestamp_(0) {}
 protected:
  uint64_t old_timestamp_;
};

class ImuManagerInterface : public ManagerInterface {
 public:
  explicit ImuManagerInterface(utils::Logger& log) : ManagerInterface(log) {}
};

}}  // namespace hyped::sensors

#endif  // SENSORS_MANAGER_INTERFACE_HPP_
