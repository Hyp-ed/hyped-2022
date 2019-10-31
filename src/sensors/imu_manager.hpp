/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 19/06/18
 * Description: IMU manager for getting IMU data from around the pod and pushes to data struct
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

#ifndef SENSORS_IMU_MANAGER_HPP_
#define SENSORS_IMU_MANAGER_HPP_

#include <cstdint>

#include "sensors/manager_interface.hpp"

#include "sensors/interface.hpp"
#include "utils/system.hpp"

namespace hyped {

using utils::Logger;

namespace sensors {
/**
 * @brief creates class to hold multiple IMUs and respective data.
 *
 */
class ImuManager: public ImuManagerInterface {
  typedef data::DataPoint<array<ImuData, data::Sensors::kNumImus>>  DataArray;

 public:
  /**
   * @brief Construct a new Imu Manager object
   *
   * @param log
   * @param imu
   */
  explicit ImuManager(Logger& log);

  /**
   * @brief Calibrate IMUs then begin collecting data.
   */
  void run() override;

 private:
  utils::System&   sys_;

  /**
   * @brief DataPoint array for all kNumImus
   */
  DataArray        sensors_imu_;

  /**
   * @brief needs to be references because run() passes directly to data struct
   */
  data::Data&      data_;

  ImuInterface*     imu_[data::Sensors::kNumImus];
};

}}  // namespace hyped::sensors

#endif  // SENSORS_IMU_MANAGER_HPP_
