#include "main_log.hpp"

#include <iostream>

namespace hyped {

typedef std::array<ImuData, data::Sensors::kNumImus> ImuDataArray;

namespace navigation {

MainLog::MainLog(const uint8_t id, Logger &log)
    : Thread(id, log),
      log_(log),
      sys_(System::getSystem()),
      data_(data::Data::getInstance())
{
  log_.INFO("NAV", "Logging initialising");
  calibrateGravity();

  for (std::size_t i = 0; i < data::Sensors::kNumImus; i++) {
    imu_loggers_[i].setup(i, sys_.run_id);
  }
  data::Navigation nav_data = data_.getNavigationData();
  nav_data.module_status    = data::ModuleStatus::kReady;
  data_.setNavigationData(nav_data);
}

void MainLog::calibrateGravity()
{
  log_.INFO("NAV", "Calibrating gravity");
  std::array<OnlineStatistics<NavigationVector>, data::Sensors::kNumImus> online_array;
  // Average each sensor over specified number of readings
  for (std::size_t i = 0; i < kNumCalibrationQueries; ++i) {
    data::DataPoint<ImuDataArray> sensor_readings = data_.getSensorsImuData();
    for (std::size_t j = 0; j < data::Sensors::kNumImus; ++j) {
      online_array[j].update(sensor_readings.value[j].acc);
    }
    Thread::sleep(1);
  }
  for (std::size_t j = 0; j < data::Sensors::kNumImus; ++j) {
    gravity_calibration_[j] = online_array[j].getMean();
    log_.INFO("NAV",
              "Update: g=(%.5f, %.5f, %.5f)",  // NOLINT
              gravity_calibration_[j][0], gravity_calibration_[j][1], gravity_calibration_[j][2]);
  }
}

void MainLog::run()
{
  log_.INFO("NAV", "Logging starting");

  while (sys_.running_) {
    data::DataPoint<ImuDataArray> sensor_readings = data_.getSensorsImuData();
    for (std::size_t i = 0; i < data::Sensors::kNumImus; ++i) {
      // Apply calibrated correction
      NavigationVector acceleration
        = sensor_readings.value[i].acc;  // TODO: .acc should be renamed to acceleration, but must
                                         // be changed in data.hpp and other areas as
      NavigationVector calibrated_acceleration = acceleration - gravity_calibration_[i];
      log_.DBG("NAV", "%.3f %.3f %.3f / %.3f %.3f %.3f", acceleration[0], acceleration[1],
               acceleration[2], calibrated_acceleration[0], calibrated_acceleration[1],
               calibrated_acceleration[2]);
      imu_loggers_[i].dataToFile(calibrated_acceleration, acceleration, sensor_readings.timestamp);
    }
  }
}

}  // namespace navigation
}  // namespace hyped
