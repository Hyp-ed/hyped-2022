#include "bms.hpp"
#include "bms_manager.hpp"
#include "fake_batteries.hpp"

#include <utils/timer.hpp>

namespace hyped::sensors {

BmsManager::BmsManager(utils::Logger log, const Config &config)
    : Thread(log),
      sys_(utils::System::getSystem()),
      data_(Data::getInstance()),
      config_(config)
{
  if (sys_.config_.use_fake_batteries) {
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      bms_[i] = new FakeBatteries(log_, true, false);
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries] = new FakeBatteries(log_, false, false);
    }
  } else if (sys_.config_.use_fake_batteries_fail) {
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      bms_[i] = new FakeBatteries(log_, true, true);
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries] = new FakeBatteries(log_, false, true);
    }
  } else {
    // create BMS LP
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      Bms *bms = new Bms(i, log_);
      bms->start();
      bms_[i] = bms;
    }
    if (!sys_.config_.use_fake_high_power) {
      for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
        bms_[i + data::Batteries::kNumLPBatteries] = new HighPowerBms(i, log_);
      }
    } else {
      for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
        bms_[i + data::Batteries::kNumLPBatteries] = new FakeBatteries(log_, false, false);
      }
    }
  }
  // kInit for state machine transition
  batteries_               = data_.getBatteriesData();
  batteries_.module_status = data::ModuleStatus::kInit;
  data_.setBatteriesData(batteries_);
  Thread::yield();
  start_time_ = utils::Timer::getTimeMicros();
  log_.info("batteries data has been initialised");
}

bool BmsManager::checkIMD()
{
  for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
    if (batteries_.high_power_batteries[i].imd_fault) {
      log_.error("IMD Fault %d: clearing imd_out_, throwing kCriticalFailure", i);
      return false;
    }
  }
  return true;
}

void BmsManager::run()
{
  // TODO(miltfra): Refactor this into stages
  while (sys_.isRunning()) {
    batteries_ = data_.getBatteriesData();

    // keep updating data_ based on values read from sensors
    for (size_t i = 0; i < data::Batteries::kNumLPBatteries; ++i) {
      batteries_.low_power_batteries.at(i) = bms_[i]->getData();
      if (!bms_[i]->isOnline()) { batteries_.low_power_batteries[i].voltage = 0; }
    }
    for (size_t i = 0; i < data::Batteries::kNumHPBatteries; ++i) {
      batteries_.high_power_batteries.at(i) = bms_[i + data::Batteries::kNumLPBatteries]->getData();
      if (!bms_[i + data::Batteries::kNumLPBatteries]->isOnline()) {
        batteries_.high_power_batteries[i].voltage = 0;
      }
    }

    // Check if BMS is ready at this point.
    // waiting time for BMS boot up is a fixed time.
    if (utils::Timer::getTimeMicros() - start_time_ > check_time_) {
      // if previous state is kInit, turn it to ready
      if (batteries_.module_status == data::ModuleStatus::kInit) {
        log_.debug("Batteries are ready");
        batteries_.module_status = data::ModuleStatus::kReady;
      }
      if (batteries_.module_status != data::ModuleStatus::kCriticalFailure) {
        if (!(batteriesInRange() && checkIMD())) {
          if (batteries_.module_status != previous_status_) log_.error("battery failure detected");
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
        log_.error("BMS LP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < 0 || battery.current > 150) {  // current in 0A to 15A
      if (batteries_.module_status != previous_status_)
        log_.error("BMS LP %d current out of range: %d", i, battery.current);
      return false;
    }

    // temperature in 0C to 70C (70C is the upper safe limit)
    // 80C would be the shutdown temperature
    if (battery.average_temperature < 0 || battery.average_temperature > 70) {
      if (batteries_.module_status != previous_status_)
        log_.error("BMS LP %d temperature out of range: %d", i, battery.average_temperature);
      return false;
    }

    if (battery.charge < 20 || battery.charge > 100) {  // charge in 20% to 100%
      if (batteries_.module_status != previous_status_)
        log_.error("BMS LP %d charge out of range: %d", i, battery.charge);
      return false;
    }
  }

  // check HP
  for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
    auto &battery = batteries_.high_power_batteries[i];  // reference batteries individually
    if (battery.voltage < 33 || battery.voltage > 42) {  // voltage in 3.3V to 4.2V
      if (batteries_.module_status != previous_status_)
        log_.error("BMS HP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < 0 || battery.current > 4000) {  // current in 0A to 400A
      if (batteries_.module_status != previous_status_)
        log_.error("BMS HP %d current out of range: %d", i, battery.current);
      return false;
    }

    if (battery.average_temperature < 0
        || battery.average_temperature > 80) {  // temperature in 0C to 80C
      if (batteries_.module_status != previous_status_)
        log_.error("BMS HP %d temperature out of range: %d", i, battery.average_temperature);
      return false;
    }

    if (battery.low_temperature < 0) {
      if (batteries_.module_status != previous_status_)
        log_.error("BMS HP %d temperature out of range: %d", i, battery.low_temperature);
      return false;
    }

    if (battery.high_temperature > 80) {
      if (batteries_.module_status != previous_status_)
        log_.error("BMS HP %d temperature out of range: %d", i, battery.high_temperature);
      return false;
    }

    if (battery.charge < 20 || battery.charge > 100) {  // charge in 20% to 100%
      if (batteries_.module_status != previous_status_)
        log_.error("BMS HP %d charge out of range: %d", i, battery.charge);
      return false;
    }
  }
  return true;
}

}  // namespace hyped::sensors
