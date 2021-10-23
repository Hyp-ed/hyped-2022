#pragma once

#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include <data/data.hpp>
#include <data/data_point.hpp>

namespace hyped {

namespace navigation {

class ImuDataLogger {
 public:
  ImuDataLogger();
  ~ImuDataLogger();
  void setup(int imu_id, int run_id);
  void setupKalman(int imu_id, int run_id);
  void dataToFileSimulation(const data::NavigationVector &acceleration, const uint32_t timestamp);
  void dataToFile(const data::NavigationVector &raw_acceleration,
                  const data::NavigationVector &calibrated_acceleration, const uint32_t timestamp);
  void dataToFileKalman(const data::NavigationVector &raw_acceleration,
                        const data::NavigationVector &calibrated_acceleration,
                        const data::NavigationVector &x, const uint32_t timestamp);

 private:
  std::string file_path_;
  std::ofstream *outfile_;
};
}  // namespace navigation
}  // namespace hyped
