/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Header for Imu data logger to write IMU data to CSV files
 *
 *  Copyright 2019 HYPED
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *  except in compliance with the License. You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under
 *  the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *  either express or implied. See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#pragma once

#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <data/data.hpp>
#include <data/data_point.hpp>
#include <fstream>
#include <string>

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
