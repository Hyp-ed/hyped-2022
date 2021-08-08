#pragma once

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
