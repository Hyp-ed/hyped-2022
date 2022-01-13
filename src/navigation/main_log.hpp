#pragma once

#include "imu_data_logger.hpp"
#include "navigation.hpp"

#include <stdio.h>

#include <cstdio>
#include <fstream>

#include <data/data.hpp>
#include <data/data_point.hpp>
#include <sensors/imu.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/math/statistics.hpp>
#include <utils/system.hpp>

namespace hyped {

namespace navigation {

class MainLog : public utils::concurrent::Thread {
 public:
  /**
   * @brief Construct a new Main Log object
   *
   * @param id log ID
   * @param log System logger
   */
  explicit MainLog(const uint8_t id, utils::Logger &log);
  void run() override;

 private:
  static constexpr int kNumCalibrationQueries = 10000;

  utils::Logger &log_;
  utils::System &sys_;
  data::Data &data_;
  std::array<data::NavigationVector, data::Sensors::kNumImus> gravity_calibration_;

  std::array<navigation::ImuDataLogger, data::Sensors::kNumImus> imu_loggers_;

  /**
   * @brief Determine the value of gravitational acceleration measured by sensors at rest
   */
  void calibrateGravity();
};

}  // namespace navigation
}  // namespace hyped
