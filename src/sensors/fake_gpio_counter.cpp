/*
 * Author: Gregory Dayao and Jack Horsburgh
 * Organisation: HYPED
 * Date: 1/4/19
 * Description:
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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include "sensors/fake_gpio_counter.hpp"
#include "utils/timer.hpp"

uint64_t kBrakeTime = 10000000;
uint32_t kTrackDistance = 2000;
double kStripeDistance = 30.48;     // metres
uint64_t kMaxTime = 1500000;     // between stripe readings before throw failure (micros)

namespace hyped {

using data::StripeCounter;
using utils::Logger;

namespace sensors {

FakeGpioCounter::FakeGpioCounter(Logger& log, bool miss_stripe)
    : log_(log),
    data_(Data::getInstance()),
    miss_stripe_(miss_stripe),
    is_from_file_(false),
    acc_ref_init_(false),
    stripe_file_timestamp_(0)

{
  stripe_count_.count.value = 0;                                      // start stripe count
  stripe_count_.operational = true;
}

FakeGpioCounter::FakeGpioCounter(Logger& log,
  bool miss_stripe, std::string file_path)
    : log_(log),
    data_(Data::getInstance()),
    miss_stripe_(miss_stripe),
    file_path_(file_path),
    is_from_file_(true),
    acc_ref_init_(false),
    stripe_file_timestamp_(0)
{
  stripe_count_.count.value = 0;                                      // start stripe count
  stripe_count_.operational = true;
  stripe_count_.count.timestamp = 0;
  readFromFile(stripe_data_);           // read text from file into vector class member
  if (miss_stripe_) {
    log_.INFO("Fake-GpioCounter", "Fake Keyence Fail initialised");
  } else {
    log_.INFO("Fake-GpioCounter", "Fake Keyence initialised");
  }
}

StripeCounter FakeGpioCounter::getStripeCounter()     // returns incorrect stripe count
{
  data::State state = data_.getStateMachineData().current_state;
  if (!acc_ref_init_ && state == data::State::kAccelerating) {
    accel_start_time_ = utils::Timer::getTimeMicros();
    acc_ref_init_ = true;
  }

  if (is_from_file_) {
    // Get time in micro seconds and iterate through the vector until we find what stripe we are at
    if ((state == data::State::kAccelerating || state == data::State::kNominalBraking)
       && acc_ref_init_) {
      uint64_t time_now_micro = (utils::Timer::getTimeMicros() - accel_start_time_);

      for (StripeCounter stripe : stripe_data_) {
        if (stripe.count.timestamp < time_now_micro) {
          stripe_count_.count.value = stripe.count.value;
          // use system timestamp from file
          stripe_count_.count.timestamp = utils::Timer::getTimeMicros();
          stripe_file_timestamp_ = stripe.count.timestamp;
        } else {
          break;
        }
      }
    }
    checkData();
    } else {
      // We are not in a state were we have data from a text file
      // base data of the navigation output
      data::Navigation nav   = data_.getNavigationData();     // throw failure from fake_imu
      uint32_t current_count = stripe_count_.count.value;

      uint16_t nav_count = std::floor(nav.distance/kStripeDistance);      // cast floor int;

      if (current_count != nav_count) {
        stripe_count_.count.value = nav_count;
        stripe_count_.count.timestamp = utils::Timer::getTimeMicros();
      }
    }
  return stripe_count_;
}

void FakeGpioCounter::checkData()
{
  if (is_from_file_) {
    uint64_t time_after = 0;
    if (acc_ref_init_) {
      time_after = utils::Timer::getTimeMicros() - (stripe_file_timestamp_ + accel_start_time_);   // NOLINT [whitespace/line_length]
    }
    log_.DBG3("Fake-GpioCounter", "time_after: %d", time_after);
    if (time_after > kMaxTime && miss_stripe_ && stripe_count_.count.value > 1) { // time_after is longer on first few stripes NOLINT [whitespace/line_length]
      log_.INFO("Fake-GpioCounter", "missed stripe!");
      stripe_count_.operational = false;
    }
  }
}

void FakeGpioCounter::readFromFile(std::vector<StripeCounter>& data)
  {
    std::ifstream data_file(file_path_, std::ifstream::in);
    float count;
    float time;
    if (data_file.is_open()) {
      // read in pairs of stripe_count, timestamp
      while (data_file >> time && data_file >> count) {
        StripeCounter this_line;
        this_line.count.value = count;
        // save timestamps in Microseconds
        this_line.count.timestamp = time*1000;
        stripe_data_.push_back(this_line);
      }
    } else {
      log_.ERR("Fake-GpioCounter", "cannot open file");
    }
    data_file.close();
  }
}}
