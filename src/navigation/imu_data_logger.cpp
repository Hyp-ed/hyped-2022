/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Imu data logger to write IMU data to CSV files
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

#include "imu_data_logger.hpp"

namespace hyped {
namespace navigation {

ImuDataLogger::ImuDataLogger()
    : file_path_(),
      outfile_(new std::ofstream())
{}

ImuDataLogger::~ImuDataLogger()
{
  outfile_->close();
}

void ImuDataLogger::setup(int imu_id, int run_id)
{
  char buff[100];
  snprintf(buff, sizeof(buff), "test_data/run%d/imu%d/data.csv", run_id, imu_id);
  file_path_ = buff;
  outfile_->open(file_path_);
  *outfile_ << "arx,ary,arz,acx,acy,acz,t\n";
}

void ImuDataLogger::setupKalman(int imu_id, int run_id)
{
  char buff[100];
  snprintf(buff, sizeof(buff), "test_data/run%d/imu%d/data.csv", run_id, imu_id);
  file_path_ = buff;
  outfile_->open(file_path_);
  *outfile_ << "arx,ary,arz,acx,acy,acz,afx,afy,afz,t\n";
}

void ImuDataLogger::dataToFileSimulation(NavigationVector& acc, uint32_t timestamp)
{
  *outfile_ << acc[0] << "," << acc[1] << "," << acc[2] << ","
            << timestamp << "\n";
}

void ImuDataLogger::dataToFile(NavigationVector& accR, NavigationVector& accC,
                   uint32_t timestamp)
{
  *outfile_ << accR[0] << "," << accR[1] << "," << accR[2] << ","
            << accC[0] << "," << accC[1] << "," << accC[2] << ","
            << timestamp << "\n";
}

void ImuDataLogger::dataToFileKalman(NavigationVector& accR,
                   NavigationVector& accC, NavigationVector& x,
                   uint32_t timestamp)
{
  *outfile_ << accR[0] << "," << accR[1] << "," << accR[2] << ","
            << accC[0] << "," << accC[1] << "," << accC[2] << ","
            << x[0]    << "," << x[1]    << "," << x[2]    << ","
            << timestamp << "\n";
}

}}  // namespace hyped navigation
