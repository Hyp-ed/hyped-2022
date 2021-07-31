/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 22/05/2019
 * Description: Main file for multi IMU logging
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
#ifndef NAVIGATION_MAIN_LOG_HPP_
#define NAVIGATION_MAIN_LOG_HPP_

#include <stdio.h>

#include <cstdio>
#include <data/data.hpp>
#include <data/data_point.hpp>
#include <fstream>
#include <sensors/imu.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/math/statistics.hpp>
#include <utils/system.hpp>

#include "imu_data_logger.hpp"
#include "navigation.hpp"

namespace hyped {

using data::Data;
using data::ImuData;
using data::NavigationVector;
using navigation::ImuDataLogger;
using utils::Logger;
using utils::System;
using utils::concurrent::Thread;
using utils::math::OnlineStatistics;

namespace navigation {

class MainLog : public Thread {
 public:
  explicit MainLog(uint8_t id, Logger &log);
  void run() override;

 private:
  static constexpr int kNumCalibrationQueries = 10000;

  Logger &log_;
  System &sys_;
  Data &data_;
  std::array<NavigationVector, data::Sensors::kNumImus> gravity_calibration_;

  std::array<ImuDataLogger, data::Sensors::kNumImus> imu_loggers_;

  /**
   * @brief Determine the value of gravitational acceleration measured by sensors at rest
   */
  void calibrateGravity();
};

}  // namespace navigation
}  // namespace hyped

#endif  // NAVIGATION_MAIN_LOG_HPP_
