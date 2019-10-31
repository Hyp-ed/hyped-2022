/*
 * Author: Gregory Dayao and Jack Horsburgh
 * Organisation: HYPED
 * Date: 6/06/19
 * Description: Main class for fake.
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

#include <stdlib.h>

#include "sensors/fake_temperature.hpp"
#include "utils/timer.hpp"

namespace hyped {
namespace sensors {

FakeTemperature::FakeTemperature(Logger& log, bool is_fail)
      : data_(Data::getInstance()),
      log_(log),
      failure_(300),
      success_(30),
      is_fail_(is_fail),
      acc_start_time_(0),
      acc_started_(false),
      failure_time_(0),
      failure_happened_(false)
{
  temp_.temp = success_;
  if (is_fail_) {
    log_.INFO("Fake-Temperature", "Fake Temperature Fail initialised");
  } else {
    log_.INFO("Fake-Temperature", "Fake Temperature initialised");
  }
}

void FakeTemperature::run()
{
  // We want to fail after we start accelerating
  // We can make it random from 0 to 20 seconds
  if (!acc_started_) {
    data::State state = data_.getStateMachineData().current_state;
    if (state == data::State::kAccelerating) {
      acc_start_time_ = utils::Timer::getTimeMicros();
      // Generate a random time for a failure
      if (is_fail_)
        failure_time_ = (rand() % 20 + 1) * 1000000;
      acc_started_ = true;
    }
  }
  checkFailure();
}

void FakeTemperature::checkFailure()
{
  if (is_fail_ && failure_time_ != 0 && !failure_happened_) {
    if (utils::Timer::getTimeMicros() - acc_start_time_ >= failure_time_) {
      temp_.temp = failure_;
      failure_happened_ = true;
    }
  }
}

int FakeTemperature::getData()
{
  return temp_.temp;
}

}}  // namespace hyped::sensors
