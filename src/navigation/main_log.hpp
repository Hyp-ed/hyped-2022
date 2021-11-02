#pragma once

#include "navigation.hpp"

#include <stdio.h>

#include <cstdio>
#include <fstream>

#include "imu_data_logger.hpp"
#include <data/data.hpp>
#include <data/data_point.hpp>
#include <sensors/imu.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/math/statistics.hpp>
#include <utils/system.hpp>

namespace hyped {

using data::ImuData;
using data::NavigationVector;
using navigation::ImuDataLogger;
using utils::Logger;
using utils::System;
using utils::concurrent::Thread;
using utils::math::OnlineStatistics;

namespace navigation {

class MainLog : public utils::concurrent::Thread {
 public:
  /**
   * @brief Construct a new Main Log object
   *
   * @param id log ID
   * @param log Logger object
   */
  explicit MainLog(uint8_t id, Logger &log);
  void run() override;

 private:
  static constexpr int kNumCalibrationQueries = 10000;

  Logger &log_;
  System &sys_;
  data::Data &data_;
  std::array<NavigationVector, data::Sensors::kNumImus> gravity_calibration_;

  std::array<ImuDataLogger, data::Sensors::kNumImus> imu_loggers_;

  /**
   * @brief Determine the value of gravitational acceleration measured by sensors at rest
   */
  void calibrateGravity();
};

}  // namespace navigation
}  // namespace hyped
