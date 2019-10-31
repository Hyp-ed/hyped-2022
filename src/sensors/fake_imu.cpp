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

#include <math.h>
#include <random>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "sensors/fake_imu.hpp"
#include "data/data_point.hpp"
#include "utils/timer.hpp"
#include "utils/math/statistics.hpp"

namespace hyped {

using utils::math::OnlineStatistics;

namespace sensors {

FakeImuFromFile::FakeImuFromFile(utils::Logger& log,
                std::string acc_file_path,
                std::string dec_file_path,
                std::string em_file_path,
                bool is_fail_acc,
                bool is_fail_dec,
                float noise)
    : log_(log),
      acc_noise_(1),
      acc_file_path_(acc_file_path),
      dec_file_path_(dec_file_path),
      em_file_path_(em_file_path),
      cal_started_(false),
      acc_started_(false),
      dec_started_(false),
      em_started_(false),
      is_fail_acc_(is_fail_acc),
      is_fail_dec_(is_fail_dec),
      failure_happened_(false),
      failure_time_acc_(0),
      failure_time_dec_(0),
      noise_(noise),
      data_(data::Data::getInstance())
{
  acc_fail_[0] = -37.3942;
  acc_fail_[1] = 0;
  acc_fail_[2] = 9.8;
  readDataFromFile(acc_file_path_, dec_file_path_, em_file_path_);
  if (is_fail_acc_) {
    log_.INFO("Fake-IMU", "Fake IMU Fail_Acc initialised");
  } else if (is_fail_dec_) {
    log_.INFO("Fake-IMU", "Fake IMU Fail_Dec initialised");
  } else {
    log_.INFO("Fake-IMU", "Fake IMU initialised");
  }
}

void FakeImuFromFile::startCal()
{
  imu_ref_time_ = utils::Timer::getTimeMicros();
  acc_count_ = 0;
}

void FakeImuFromFile::startAcc()
{
  imu_ref_time_ = utils::Timer::getTimeMicros();
  acc_count_ = 0;
}

void FakeImuFromFile::startDec()
{
  imu_ref_time_ = utils::Timer::getTimeMicros();
  acc_count_ = 0;
}

void FakeImuFromFile::startEm()
{
  imu_ref_time_ = utils::Timer::getTimeMicros();
  acc_count_ =  0;
}

void FakeImuFromFile::setFailure(data::State& state)
{
  // Random point of failure after acc from 0 to 20 seconds
  if (state == data::State::kAccelerating && is_fail_acc_) {
    // Generate a random time for a failure
    failure_time_acc_ = (rand() % 20 + 1) * 1000000;
  }
  // Random point of failure after dec from 0 to 10 seconds
  if (state == data::State::kNominalBraking && is_fail_dec_) {
    // Generate a random time for a failure
    failure_time_dec_ = (rand() % 10 + 1) * 1000000;
  }
}

void FakeImuFromFile::getData(ImuData* imu)
{
  data::State state = data_.getStateMachineData().current_state;
  bool operational = true;

  if (failure_time_acc_ == 0 || failure_time_dec_ == 0) {
    setFailure(state);
  }

  if (state == data::State::kCalibrating) {
    // start cal
    if (!cal_started_) {
      log_.INFO("Fake-IMU", "Start calibrating ...");
      cal_started_ = true;
      startCal();
    }

    // pod stationary
    prev_acc_ = getZeroAcc();

  } else if (state == data::State::kAccelerating) {
    // start acc
    if (!acc_started_) {
      log_.INFO("Fake-IMU", "Start accelerating ...");
      acc_started_ = true;
      startAcc();
    }

    if (accCheckTime()) {
      acc_count_ = std::min(acc_count_, (int64_t) acc_val_read_.size());
      // Check so you don't go out of bounds
      if (acc_count_ == (int64_t) acc_val_read_.size()) {
        prev_acc_ = acc_val_read_[acc_count_- 1];
        operational = acc_val_operational_[acc_count_ - 1];
      } else {
        prev_acc_ = acc_val_read_[acc_count_];
        operational = acc_val_operational_[acc_count_];
      }
      if (is_fail_acc_) {
        if (utils::Timer::getTimeMicros() - imu_ref_time_ >= failure_time_acc_ || failure_happened_) { // NOLINT [whitespace/line_length]
          if (!failure_happened_) {
            log_.INFO("Fake-IMU", "Start failure...");
          }
          prev_acc_ = acc_fail_;
          operational = false;
          failure_happened_ = true;
        }
      }
    }

  } else if (state == data::State::kNominalBraking) {
    if (!dec_started_) {
      log_.INFO("Fake-IMU", "Start decelerating...");
      dec_started_ = true;
      startDec();
    }

    if (accCheckTime()) {
      acc_count_ = std::min(acc_count_, (int64_t) dec_val_read_.size());
      // Check so you don't go out of bounds
      if (acc_count_ == (int64_t) dec_val_read_.size()) {
        prev_acc_ = dec_val_read_[acc_count_-1];
        operational = dec_val_operational_[acc_count_-1];
      } else {
        prev_acc_ = dec_val_read_[acc_count_];
        operational = dec_val_operational_[acc_count_];
      }
      if (is_fail_dec_) {
        if (utils::Timer::getTimeMicros() - imu_ref_time_ >= failure_time_dec_ || failure_happened_) { // NOLINT [whitespace/line_length]
          if (!failure_happened_) {
            log_.INFO("Fake-IMU", "Start failure...");
          }
          prev_acc_ = acc_fail_;
          operational = false;
          failure_happened_ = true;
        }
      }

      // prevent acc from becoming negative when pod is stopping
      float vel = data_.getNavigationData().velocity;
      log_.DBG3("Fake-IMU", "velocity: %f", vel);
      if (vel < 1) {
        prev_acc_ = getZeroAcc();
      }
    }

  } else if (state == data::State::kEmergencyBraking) {
    if (!em_started_) {
      log_.INFO("Fake-IMU", "Start emergency breaking...");
      em_started_ = true;
      startEm();
    }

    if (accCheckTime()) {
      acc_count_ = std::min(acc_count_, (int64_t) em_val_read_.size());

      // Check so you don't go out of bounds
      if (acc_count_ == (int64_t) em_val_read_.size()) {
        prev_acc_ = em_val_read_[acc_count_-1];
      } else {
        prev_acc_ = em_val_read_[acc_count_];
      }
      float vel = data_.getNavigationData().velocity;
      log_.DBG3("Fake-IMU", "velocity: %f", vel);
      // prevent acc from becoming negative when pod is stopping
      if (vel < 1) {
        prev_acc_ = getZeroAcc();
      }
      operational = true;
    }

  } else {
    prev_acc_ = getZeroAcc();
    operational = true;
  }
  imu->acc = prev_acc_;
  imu->operational = operational;
}

NavigationVector FakeImuFromFile::addNoiseToData(NavigationVector value, float noise)
{
  NavigationVector temp;
  static std::default_random_engine generator;

  for (int i = 0; i < 3; i++) {
    std::normal_distribution<NavigationType> distribution(value[i], noise);
    temp[i] = distribution(generator);
  }
  return temp;
}

void FakeImuFromFile::readDataFromFile(std::string acc_file_path,
                               std::string dec_file_path,
                               std::string em_file_path)
{
  for (int i = 0; i < 3; i++) {
    std::string file_path;
    uint32_t timestamp;
    std::vector<NavigationVector>* val_read;
    std::vector<bool>* bool_read;

    if (i == 0) {
      file_path = acc_file_path;
      timestamp = kAccTimeInterval;
      val_read  = &acc_val_read_;
      bool_read = &acc_val_operational_;
    } else if (i == 1) {
      file_path = dec_file_path;
      timestamp = kAccTimeInterval;
      val_read  = &dec_val_read_;
      bool_read = &dec_val_operational_;
    } else if (i == 2) {
      file_path = em_file_path;
      timestamp = kAccTimeInterval;
      val_read  = &em_val_read_;
      bool_read = &em_val_operational_;
    }
    std::ifstream file;
    file.open(file_path);
    if (!file.is_open()) {
      log_.ERR("Fake-IMU", "Wrong file path for argument: %d", i);
    }

    NavigationVector value;
    int counter = 0;
    uint32_t temp_time;
    std::string line;

    while (getline(file, line)) {
      std::stringstream input(line);
      input >> temp_time;

      // checks whether timestamp format matches refresh rate
      if (temp_time != timestamp*counter) {
        log_.ERR("Fake-IMU", "Timestamp format invalid %d", temp_time);
      }

      input >> value[0];
      value[1] = 0.0;
      value[2] = 9.8;

      val_read->push_back(addNoiseToData(value, noise_));
      bool_read->push_back(1);      // always true

      counter++;
    }

    file.close();
  }
}

bool FakeImuFromFile::accCheckTime()
{
  uint64_t now = utils::Timer::getTimeMicros();
  uint64_t time_span = (now - imu_ref_time_) / 1000;

  if (time_span < kAccTimeInterval*acc_count_) {
    return false;
  }
  acc_count_ = time_span/kAccTimeInterval + 1;
  return true;
}

NavigationVector FakeImuFromFile::getZeroAcc()
{
  NavigationVector value;
  value[0] = 0.0;
  value[1] = 0.0;
  value[2] = 9.8;
  return addNoiseToData(value, noise_);
}


// FakeAccurateImu::FakeAccurateImu(utils::Logger& log)
//     : data_(data::Data::getInstance()),
//       acc_noise_(1),
//       log_(log)
// { /* EMPTY */ }

// void FakeAccurateImu::getData(ImuData* imu)
// {
//   data::Navigation nav = data_.getNavigationData();
//   data::Motors     mot = data_.getMotorData();
//   data::StateMachine      stm = data_.getStateMachineData();

//   if (stm.current_state == data::State::kEmergencyBraking) {
//     imu->acc[0] = -25;
//   } else {
//     // get average rmp
//     double rpm = 0;
//     rpm += mot.velocity_1;
//     rpm += mot.velocity_2;
//     rpm += mot.velocity_3;
//     rpm += mot.velocity_4;
//     rpm /= 4;

//     // get angular velocity
//     double velocity = (rpm*2*3.14159265358979323846*0.148)/60;
//     uint32_t scale = 4;
//     if (!std::isnan(nav.velocity))
//       imu->acc[0] = (velocity - nav.velocity)/scale;
//     else
//       imu->acc[0] = 0.0;
//   }
//   imu->acc[1] = 0;
//   imu->acc[2] = 9.8;

//   imu->acc = FakeImuFromFile::addNoiseToData(imu->acc, acc_noise_);
//   imu->operational = true;
// }

}}  // namespace hyped::sensors
