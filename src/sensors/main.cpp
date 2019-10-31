/*
 * Author:
 * Organisation: HYPED
 * Date:
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

#include "sensors/main.hpp"
#include "sensors/imu_manager.hpp"
#include "sensors/bms_manager.hpp"
#include "sensors/gpio_counter.hpp"
#include "sensors/temperature.hpp"
#include "sensors/fake_gpio_counter.hpp"
#include "sensors/fake_temperature.hpp"
#include "utils/config.hpp"

namespace hyped {

using hyped::utils::concurrent::Thread;
using utils::System;
using data::Data;
using data::Sensors;
using data::StripeCounter;
using utils::io::GPIO;

namespace sensors {

Main::Main(uint8_t id, utils::Logger& log)
  : Thread(id, log),
    data_(data::Data::getInstance()),
    sys_(utils::System::getSystem()),
    log_(log),
    pins_ {static_cast<uint8_t>(sys_.config->sensors.KeyenceL), static_cast<uint8_t>(sys_.config->sensors.KeyenceR)}, // NOLINT
    imu_manager_(new ImuManager(log)),
    battery_manager_(new BmsManager(log))
{
  if (!(sys_.fake_keyence || sys_.fake_keyence_fail)) {
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      GpioCounter* keyence = new GpioCounter(log_, pins_[i]);
      keyence->start();
      keyences_[i] = keyence;
    }
  } else if (sys_.fake_keyence_fail) {
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      // miss four stripes in a row after 20th, 2000 micros during peak velocity
      keyences_[i] = new FakeGpioCounter(log_, true, "data/in/gpio_counter_fail_run.txt");
    }
  } else {
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      keyences_[i] = new FakeGpioCounter(log_, false, "data/in/gpio_counter_normal_run.txt");
    }
  }
  if (!(sys_.fake_temperature || sys_.fake_temperature_fail)) {
    temperature_ = new Temperature(log_, sys_.config->sensors.Thermistor);
  } else if (sys_.fake_temperature_fail) {
    // fake temperature fail case
    temperature_ = new FakeTemperature(log_, true);
  } else {
    temperature_ = new FakeTemperature(log_, false);
  }

  // kInit for SM transition
  sensors_ = data_.getSensorsData();
  sensors_.module_status = data::ModuleStatus::kInit;
  data_.setSensorsData(sensors_);
  log_.INFO("Sensors", "Sensors have been initialised");
}

bool Main::keyencesUpdated()
{
  for (int i = 0; i < data::Sensors::kNumKeyence; i ++) {
    if (prev_keyence_stripe_count_arr_[i].count.value != keyence_stripe_counter_arr_[i].count.value)
      return true;
  }
  return false;
}

void Main::checkTemperature()
{
  temperature_->run();               // not a thread
  data_.setTemperature(temperature_->getData());
  if (data_.getTemperature() > 85 && !log_error_) {
    log_.INFO("Sensors", "PCB temperature is getting a wee high...sorry Cheng");
    log_error_ = true;
  }
}

void Main::run()
{
  // start all managers
  battery_manager_->start();
  imu_manager_->start();

  // Initalise the keyence arrays
  keyence_stripe_counter_arr_    = data_.getSensorsData().keyence_stripe_counter;
  prev_keyence_stripe_count_arr_ = keyence_stripe_counter_arr_;

  int temp_count = 0;
  while (sys_.running_) {
    // We need to read the gpio counters and write to the data structure
    // If previous is not equal to the new data then update
    if (keyencesUpdated()) {
      // Update data structure, make prev reading same as this reading
      data_.setSensorsKeyenceData(keyence_stripe_counter_arr_);
      prev_keyence_stripe_count_arr_ = keyence_stripe_counter_arr_;
    }
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      keyence_stripe_counter_arr_[i] = keyences_[i]->getStripeCounter();
    }
    Thread::sleep(10);  // Sleep for 10ms
    temp_count++;
    if (temp_count % 20 == 0) {       // check every 20 cycles of main
      checkTemperature();
      // So that temp_count does not get huge
      temp_count = 0;
    }
  }

  imu_manager_->join();
  battery_manager_->join();
}
}}
