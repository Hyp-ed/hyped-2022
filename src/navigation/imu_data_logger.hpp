#pragma once

#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>

#include <data/data.hpp>
#include <data/data_point.hpp>

namespace hyped {
using data::DataPoint;
using data::nav_t;
using data::NavigationVector;

namespace navigation {

class ImuDataLogger {
 public:
  ImuDataLogger();
  ~ImuDataLogger();
  void setup(int imu_id, int run_id);
  void setupKalman(int imu_id, int run_id);
  void dataToFileSimulation(NavigationVector &acc, uint32_t timestamp);
  void dataToFile(NavigationVector &accRaw, NavigationVector &accCor, uint32_t timestamp);
  void dataToFileKalman(NavigationVector &accRaw, NavigationVector &accCor, NavigationVector &x,
                        uint32_t timestamp);

 private:
  std::string file_path_;
  std::ofstream *outfile_;
};
}  // namespace navigation
}  // namespace hyped
