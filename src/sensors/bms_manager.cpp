#include "bms.hpp"
#include "bms_manager.hpp"
#include "fake_batteries.hpp"

#include <utils/config.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

BmsManager::BmsManager(Logger &log)
    : Thread(log),
      sys_(utils::System::getSystem()),
      data_(Data::getInstance())
{
  check_time_ = sys_.config->sensors.checktime;
  if (!(sys_.fake_batteries || sys_.fake_batteries_fail)) {
    // create BMS LP
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      Bms *bms = new Bms(i, log_);
      bms->start();
      bms_[i] = bms;
    }
    // fake HP for state machine tests
    if (!sys_.fake_highpower) {
      // create BMS HP
      for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
        bms_[i + data::Batteries::kNumLPBatteries] = new HighPowerBms(i, log_);
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

  // kInit for state machine transition
  batteries_               = data_.getBatteriesData();
  batteries_.module_status = data::ModuleStatus::kInit;
  data_.setBatteriesData(batteries_);
  Thread::yield();
  start_time_ = utils::Timer::getTimeMicros();
  log_.INFO("BMS-MANAGER", "batteries data has been initialised");
}

bool BmsManager::checkIMD()
{
  for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
    if (batteries_.high_power_batteries[i].imd_fault) {
      log_.ERR("BMS-MANAGER", "IMD Fault %d: clearing imd_out_, throwing kCriticalFailure", i);
      return false;
    }
  }
  return true;
}

void BmsManager::run()
{
  // TODO(miltfra): Refactor this into stages
  while (sys_.running_) {
    batteries_ = data_.getBatteriesData();

    // keep updating data_ based on values read from sensors
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      bms_[i]->getData(batteries_.low_power_batteries[i]);
      if (!bms_[i]->isOnline()) batteries_.low_power_batteries[i].voltage = 0;
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries]->getData(batteries_.high_power_batteries[i]);
      if (!bms_[i + data::Batteries::kNumLPBatteries]->isOnline())
        batteries_.high_power_batteries[i].voltage = 0;
    }

    // Check if BMS is ready at this point.
    // waiting time for BMS boot up is a fixed time.
    if (utils::Timer::getTimeMicros() - start_time_ > check_time_) {
      // if previous state is kInit, turn it to ready
      if (batteries_.module_status == data::ModuleStatus::kInit) {
        log_.DBG1("BMS-MANAGER", "Batteries are ready");
        batteries_.module_status = data::ModuleStatus::kReady;
      }
      if (batteries_.module_status != data::ModuleStatus::kCriticalFailure) {
        if (!(batteriesInRange() && checkIMD())) {
          if (batteries_.module_status != previous_status_)
            log_.ERR("BMS-MANAGER", "battery failure detected");
          batteries_.module_status = data::ModuleStatus::kCriticalFailure;
        }
        previous_status_ = batteries_.module_status;
      }
    }

    // publish the new data
    data_.setBatteriesData(batteries_);

    sleep(100);
  }
}

bool BmsManager::batteriesInRange()
{
  // check LP
  for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
    auto &battery = batteries_.low_power_batteries[i];   // reference batteries individually
    if (battery.voltage < 30 || battery.voltage > 37) {  // voltage in 3.0 V to 3.7 V
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS LP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < 0 || battery.current > 150) {  // current in 0A to 15A
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS LP %d current out of range: %d", i, battery.current);
      return false;
    }

    // temperature in 0C to 70C (70C is the upper safe limit)
    // 80C would be the shutdown temperature
    if (battery.average_temperature < 0 || battery.average_temperature > 70) {
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS LP %d temperature out of range: %d", i,
                 battery.average_temperature);
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
    auto &battery = batteries_.high_power_batteries[i];  // reference batteries individually
    if (battery.voltage < 33 || battery.voltage > 42) {  // voltage in 3.3V to 4.2V
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS HP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < 0 || battery.current > 4000) {  // current in 0A to 400A
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS HP %d current out of range: %d", i, battery.current);
      return false;
    }

    if (battery.average_temperature < 0
        || battery.average_temperature > 80) {  // temperature in 0C to 80C
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i,
                 battery.average_temperature);
      return false;
    }

    if (battery.low_temperature < 0) {
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i,
                 battery.low_temperature);
      return false;
    }

    if (battery.high_temperature > 80) {
      if (batteries_.module_status != previous_status_)
        log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i,
                 battery.high_temperature);
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

}  // namespace hyped::sensors
