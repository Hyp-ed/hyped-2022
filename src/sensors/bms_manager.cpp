/*
 * Author: Gregory Dayao and Jack Horsburgh
 * Organisation: HYPED
 * Date: 20/06/18
 * Description:
 * BMS manager for getting battery data and pushes to data struct.
 * Checks whether batteries are in range and enters emergency state if fails.
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


#include "sensors/bms_manager.hpp"

#include "sensors/bms.hpp"
#include "utils/timer.hpp"
#include "sensors/fake_batteries.hpp"
#include "utils/config.hpp"

namespace hyped {
namespace sensors {

BmsManager::BmsManager(Logger& log)
    : Thread(log),
      sys_(utils::System::getSystem()),
      data_(Data::getInstance())
{
  check_time_ = sys_.config->sensors.checktime;
  if (!(sys_.fake_batteries || sys_.fake_batteries_fail)) {
    // create BMS LP
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      BMS* bms = new BMS(i, log_);
      bms->start();
      bms_[i] = bms;
    }
    // fake HP for state machine tests
    if (!sys_.fake_highpower) {
      // create BMS HP
      for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
        bms_[i + data::Batteries::kNumLPBatteries] = new BMSHP(i, log_);
      }
    } else {
      // fake HP battery only
      for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
        bms_[i + data::Batteries::kNumLPBatteries] = new FakeBatteries(log_, false, false);
      }
    }
  } else if (sys_.fake_batteries_fail) {
    // fake batteries fail here
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      bms_[i] = new FakeBatteries(log_, true, true);
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries] = new FakeBatteries(log_, false, true);
    }
  } else {
    // fake batteries here
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      bms_[i] = new FakeBatteries(log_, true, false);
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries] = new FakeBatteries(log_, false, false);
    }
  }

  // kInit for SM transition
  batteries_ = data_.getBatteriesData();
  batteries_.module_status = data::ModuleStatus::kInit;
  data_.setBatteriesData(batteries_);
  Thread::yield();
  start_time_ = utils::Timer::getTimeMicros();
  log_.INFO("BMS-MANAGER", "batteries data has been initialised");
}

bool BmsManager::checkIMD()
{
  for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
    if (batteries_.high_power_batteries[i].imd_fault == false) {
      log_.ERR("BMS-MANAGER", "IMD Fault %d: clearing imd_out_, throwing kCriticalFailure", i);
      return false;
    }
  }
  return true;
}

void BmsManager::run()
{
  while (sys_.running_) {
    // keep updating data_ based on values read from sensors
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      bms_[i]->getData(&batteries_.low_power_batteries[i]);
      if (!bms_[i]->isOnline())
        batteries_.low_power_batteries[i].voltage = 0;
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries]->getData(&batteries_.high_power_batteries[i]);
      if (!bms_[i + data::Batteries::kNumLPBatteries]->isOnline())
        batteries_.high_power_batteries[i].voltage = 0;
    }

    /*  THIS HAS TO WORK LATER pls
    if (utils::Timer::getTimeMicros() - start_time_ > check_time_) {
      // check health of batteries
      if (batteries_.module_status != data::ModuleStatus::kCriticalFailure) {
        if (!(batteriesInRange() && checkIMD())) {
          if (batteries_.module_status != previous_status_)
            log_.ERR("BMS-MANAGER", "battery failure detected");
          batteries_.module_status = data::ModuleStatus::kCriticalFailure;
        }
        previous_status_ = batteries_.module_status;
      }
    }
    */

    // publish the new data
    data_.setBatteriesData(batteries_);

    sleep(100);
  }
}

bool BmsManager::batteriesInRange()
{
  // check LP
  for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
    auto& battery = batteries_.low_power_batteries[i];      // reference batteries individually
    if (battery.voltage < 175 || battery.voltage > 294) {   // voltage in 17.5V to 29.4V
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS LP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < 0 || battery.current > 500) {       // current in 0A to 50A
       if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS LP %d current out of range: %d", i, battery.current);
      return false;
    }

    if (battery.average_temperature < 10 || battery.average_temperature > 60) {  // temperature in 10C to 60C NOLINT[whitespace/line_length]
       if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS LP %d temperature out of range: %d", i, battery.average_temperature); // NOLINT[whitespace/line_length]
      return false;
    }

    if (battery.charge < 20 || battery.charge > 100) {  // charge in 20% to 100%
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS LP %d charge out of range: %d", i, battery.charge);
      return false;
    }
  }

  // check HP
  for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
    auto& battery = batteries_.high_power_batteries[i];     // reference batteries individually
    if (battery.voltage < 1000 || battery.voltage > 1296) {   // voltage in 100V to 129.6V
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS HP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < 0 || battery.current > 3500) {  // current in 0A to 350A
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS HP %d current out of range: %d", i, battery.current);
      return false;
    }

    if (battery.average_temperature < 10 || battery.average_temperature > 65) {  // temperature in 10C to 65C NOLINT[whitespace/line_length]
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i, battery.average_temperature); // NOLINT[whitespace/line_length]
      return false;
    }

    if (battery.low_temperature < 10) {
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i, battery.low_temperature); // NOLINT[whitespace/line_length]
      return false;
    }

    if (battery.high_temperature > 65) {
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i, battery.high_temperature); // NOLINT[whitespace/line_length]
      return false;
    }

    if (battery.charge < 20 || battery.charge > 100) {  // charge in 20% to 100%
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS HP %d charge out of range: %d", i, battery.charge);
      return false;
    }
  }
  return true;
}

}}  // namespace hyped::sensors
