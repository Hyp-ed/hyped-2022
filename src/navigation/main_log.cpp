#include "main_log.hpp"

#include <iostream>

namespace hyped {

using ImuDataArray = std::array<data::ImuData, data::Sensors::kNumImus>;

namespace navigation {

MainLog::MainLog(const uint8_t id, utils::Logger &log)
    : utils::concurrent::Thread(id, log),
      log_(log),
      sys_(utils::System::getSystem()),
      data_(data::Data::getInstance())
{
  log_.INFO("NAV", "Logging initialising");
  calibrateGravity();

  for (std::size_t i = 0; i < data::Sensors::kNumImus; i++) {
    imu_loggers_.at(i).setup(i, sys_.run_id);
  }
  data::Navigation nav_data = data_.getNavigationData();
  nav_data.module_status    = data::ModuleStatus::kReady;
  data_.setNavigationData(nav_data);
}

void MainLog::calibrateGravity()
{
  log_.INFO("NAV", "Calibrating gravity");
  std::array<utils::math::OnlineStatistics<data::NavigationVector>, data::Sensors::kNumImus>
    online_array;
  // Average each sensor over specified number of readings
  for (std::size_t i = 0; i < kNumCalibrationQueries; ++i) {
    data::DataPoint<ImuDataArray> sensor_readings = data_.getSensorsImuData();
    for (std::size_t j = 0; j < data::Sensors::kNumImus; ++j) {
      online_array.at(i).update(sensor_readings.value.at(j).acc);
    }
    utils::concurrent::Thread::sleep(1);
  }
  for (std::size_t j = 0; j < data::Sensors::kNumImus; ++j) {
    gravity_calibration_.at(j) = online_array.at(j).getMean();
    log_.INFO("NAV", "Update: g=(%.5f, %.5f, %.5f)", gravity_calibration_.at(j)[0],
              gravity_calibration_.at(j)[1], gravity_calibration_.at(j)[2]);
  }
}

void MainLog::run()
{
  log_.INFO("NAV", "Logging starting");

  while (sys_.running_) {
    data::DataPoint<ImuDataArray> sensor_readings = data_.getSensorsImuData();
    for (std::size_t i = 0; i < data::Sensors::kNumImus; ++i) {
      // Apply calibrated correction
      data::NavigationVector acceleration 
        = sensor_readings.value.at(i).acc; // TODO: .acc should be renamed to acceleration, but must be changed in data.hpp and other areas
      data::NavigationVector calibrated_acceleration = acceleration - gravity_calibration_.at(i);
      log_.DBG("NAV", "%.3f %.3f %.3f / %.3f %.3f %.3f", acceleration[0], acceleration[1],
               acceleration[2], calibrated_acceleration[0], calibrated_acceleration[1],
               calibrated_acceleration[2]);
      imu_loggers_.at(i).dataToFile(calibrated_acceleration, acceleration,
                                    sensor_readings.timestamp);
    }
  }
}

}  // namespace navigation
}  // namespace hyped
