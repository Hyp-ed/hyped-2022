#pragma once

#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <fstream>
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
  void dataToFileSimulation(data::NavigationVector &acceleration, uint32_t timestamp);
  void dataToFile(data::NavigationVector &raw_acceleration,
                  data::NavigationVector &calibrated_acceleration, uint32_t timestamp);
  void dataToFileKalman(data::NavigationVector &raw_acceleration,
                        data::NavigationVector &calibrated_acceleration, data::NavigationVector &x,
                        uint32_t timestamp);

 private:
  std::string file_path_;
  std::ofstream *outfile_;
};
}  // namespace navigation
}  // namespace hyped
