/*
 * Author:
 * Organisation: HYPED
 * Date: 11/03/2019
 * Description: Main class for fake IMUs
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#ifndef SENSORS_FAKE_IMU_HPP_
#define SENSORS_FAKE_IMU_HPP_

#include <string>
#include <vector>

#include "sensors/interface.hpp"
#include "utils/logger.hpp"

namespace hyped {

using data::ImuData;
using data::DataPoint;
using data::NavigationType;
using data::NavigationVector;

namespace sensors {

/*
 * @brief    This class is to imitate an IMU. This works by calling the constructor once
 *           and calling getData function multiple times at different time periods to produce
 *           reading that will be used by other classes.
 */
class FakeImuFromFile : public ImuInterface {
 public:
  /**
   * @brief A constructor for the fake IMU class by reading from file
   *
   * The line format of the input file would be the following
   *
   *               timestamp value_x
   *               value_y = 9.8 and value_z = 0 set by class
   *
   *               Sample of the format is located at 'src/fake_imu_input_xxx.txt'. Note that the
   *               timestamp for accelerometer has to start with 0 and must be multiples of 250
   *               for accelerometer. You must include every timestamp from 0 to the last timestamp
   *               which will be a multiple of 250.
   *
   * @param log_
   * @param acc_file_path
   * @param dec_file_path
   * @param em_file_path
   * @param is_fail_acc
   * @param is_fail_dec
   * @param noise
   */
  FakeImuFromFile(utils::Logger& log_,
          std::string acc_file_path,
          std::string dec_file_path,
          std::string em_file_path,
          bool is_fail_acc,
          bool is_fail_dec,
          float noise = 0.2);

  bool isOnline() override { return true; }

  /*
   * @brief     A function that gets the imu data at the time of call. The function will return
   *            the same data point if the time period since the last update isn't long enough. It
   *            will also skip a couple of data points if the time since the last call has been
   *            sufficiently long.
   */
  void getData(ImuData* imu) override;

  /*
   * @brief     A function that adds noise to the imu data using normal distribution
   *
   * @param[in] value    This is the mean of the normal distribution
   * @param[in] noise    This is the standard deviation of the normal distribution
   *
   * @return    Returns random data point value
   */
  static NavigationVector addNoiseToData(NavigationVector value, float noise);

 private:
  utils::Logger&       log_;
  const uint64_t kAccTimeInterval = 50;
  void startCal();
  void startAcc();
  void startDec();
  void startEm();

  /**
   * @brief sets failure time for acc or dec configuration
   * @param state current state
   */
  void setFailure(data::State& state);

  /**
   * @return NavigationVector zero acceleration as a vector
   */
  NavigationVector getZeroAcc();

  /*
   * @brief     A function that reads data from file directory. This function also validates them
   *            by checking if
   *              1) The timestamp values are valid. Multiples of 250.
   *              2) The file follows the format given in the comments of the constructor above.
   *              3) The file exists.
   *
   * @param[in]    The file format is as stated in the constructor comments
   */
  void readDataFromFile(std::string acc_file_path,
                        std::string dec_file_path,
                        std::string em_file_path);

  /*
   * @brief     Checks to see if sufficient time has pass for the sensor to be updated and checks if
   *            some data points need to be skipped
   */
  bool accCheckTime();

  NavigationVector acc_noise_;
  NavigationVector prev_acc_;
  NavigationVector acc_fail_;


  std::vector<NavigationVector> acc_val_read_;
  std::vector<bool>             acc_val_operational_;
  std::vector<NavigationVector> dec_val_read_;
  std::vector<bool>             dec_val_operational_;
  std::vector<NavigationVector> em_val_read_;
  std::vector<bool>             em_val_operational_;

  /**
   * @brief used in accCheckTime()
   */
  int64_t acc_count_;

  /**
   * @brief scales time based on getTimeMicros() and timestamps from file
   */
  uint64_t imu_ref_time_;
  std::string acc_file_path_;
  std::string dec_file_path_;
  std::string em_file_path_;
  bool cal_started_;
  bool acc_started_;
  bool dec_started_;
  bool em_started_;
  bool is_fail_acc_;
  bool is_fail_dec_;
  bool failure_happened_;
  uint64_t failure_time_acc_;
  uint64_t failure_time_dec_;
  float noise_;
  data::Data&  data_;
};

// todo - implement different options for fake imus

// class FakeAccurateImu: public ImuInterface {
//  public:
//   explicit FakeAccurateImu(utils::Logger& log_);

//   bool isOnline() override { return true; }
//   void getData(ImuData* imu) override;

//  private:
//   data::Data&    data_;
//   NavigationVector acc_noise_;
//   utils::Logger& log_;
// };

}}  // namespace hyped::sensors


#endif  // SENSORS_FAKE_IMU_HPP_
