/*
 * Author: Uday Patel, Jack Horsburgh and Ragnor Comerford
 * Organisation: HYPED
 * Date: 28/05/18
 * Description: Main sensor interfaces, used to create fake sensors
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

#ifndef SENSORS_INTERFACE_HPP_
#define SENSORS_INTERFACE_HPP_

#include <data/data.hpp>
#include <string>

namespace hyped {

using data::BatteryData;
using data::ImuData;
using data::NavigationVector;
using data::StripeCounter;
using data::TemperatureData;

namespace sensors {

class SensorInterface {
 public:
  /**
   * @brief Check if sensor is responding, i.e. connected to the system
   * @return true - if sensor is online
   */
  virtual bool isOnline() = 0;
};

class ImuInterface : public SensorInterface {
 public:
  /**
   * @brief Get IMU data
   * @param imu - output pointer to be filled by this sensor
   */
  virtual void getData(ImuData *imu) = 0;
};

class GpioInterface : public SensorInterface {
 public:
  /**
   * @brief Get GPIO data
   * @param stripe_counter - output pointer
   */
  virtual void getData(StripeCounter *stripe_counter) = 0;
};

class BMSInterface : public SensorInterface {
 public:
  /**
   * @brief Get Battery data
   * @param battery - output pointer to be filled by this sensor
   */
  virtual void getData(BatteryData *battery) = 0;
};

class TemperatureInterface {
 public:
  /**
   * @brief not a thread, checks temperature
   */
  virtual void run() = 0;

  /**
   * @brief returns int representation of temperature
   * @return int temperature degrees C
   */
  virtual int getData() = 0;
};
}  // namespace sensors
}  // namespace hyped

#endif  // SENSORS_INTERFACE_HPP_
