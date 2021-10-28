#pragma once

#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <string>

#include <data/data.hpp>
#include <data/data_point.hpp>

namespace hyped {

namespace navigation {

class ImuDataLogger {
 public:
  ImuDataLogger();
  ~ImuDataLogger();

  /**
   * @brief setup outfile
   *
   * @param imu_id ID for IMU
   * @param run_id ID for run
   */
  void setup(int imu_id, int run_id);

  /**
   * @brief setup outfile for Kalman
   *
   * @param imu_id ID for IMU
   * @param run_id ID for run
   */
  void setupKalman(int imu_id, int run_id);

  /**
   * @brief write simulation acceleration to the outfile
   *
   * @param acceleration a vector of type nav_t (float) holding acceleration
   * @param timestamp time that data was received
   */
  void dataToFileSimulation(const data::NavigationVector &acceleration, const uint32_t timestamp);

  /**
   * @brief write raw and calibrated accelerations to outfile
   *
   * @param raw_acceleration a vector of type nav_t (float) holding the raw acceleration
   * @param calibrated_acceleration a vector of type nav_t (float) holding the acceleration after
   * filtering
   * @param timestamp time that data was received
   */
  void dataToFile(const data::NavigationVector &raw_acceleration,
                  const data::NavigationVector &calibrated_acceleration, const uint32_t timestamp);

  /**
   * @brief write raw, calibrated and x accelerations to outfile
   *
   * @param raw_acceleration a vector of type nav_t (float) holding the raw acceleration
   * @param calibrated_acceleration a vector of type nav_t (float) holding the acceleration after
   * filtering
   * @param x // (TODO - rename x, we are currently unsure what exactly it is and what it is used
   * for)
   * @param timestamp time that data was received
   */
  void dataToFileKalman(const data::NavigationVector &raw_acceleration,
                        const data::NavigationVector &calibrated_acceleration,
                        const data::NavigationVector &x, const uint32_t timestamp);

 private:
  std::string file_path_;
  std::ofstream *outfile_;
};
}  // namespace navigation
}  // namespace hyped
