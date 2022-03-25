#include "fake_batteries.hpp"

#include <stdlib.h>

#include <data/data.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

FakeBatteries::FakeBatteries(utils::Logger &log, bool is_lp, bool is_fail)
    : data_(data::Data::getInstance()),
      log_(log),
      lp_failure_{{100, 1000, 0, 0, 100, 0, 0, 0}},
      lp_success_{{245, 450, 70, 0, 35, 0, 0, 0}},
      hp_failure_{{900, 10000, 0, 5, 70, 100, 500, 400}},
      hp_success_{{1150, 1000, 70, 30, 35, 40, 3000, 3200}},
      cases_{{lp_failure_, lp_success_, hp_failure_, hp_success_}},
      is_lp_(is_lp),
      is_fail_(is_fail),
      acc_start_time_(0),
      acc_started_(false),
      failure_time_(0),
      failure_happened_(false)
{
  if (is_lp_) {
    case_index_ = 1;
  } else {
    case_index_ = 3;
  }
  updateBatteryData();
  if (is_lp_) {
    if (is_fail_) {
      log_.info("Fake LPBattery Fail initialised");
    } else {
      log_.info("Fake LPBattery initialised");
    }
  } else {
    if (is_fail_) {
      log_.info("Fake HPBattery Fail initialised");
    } else {
      log_.info("Fake HPBattery initialised");
    }
  }
}

data::BatteryData FakeBatteries::getData()
{
  // We want to fail after we start accelerating
  // We can make it random from 0 to 20 seconds
  if (!acc_started_) {
    data::State state = data_.getStateMachineData().current_state;
    if (state == data::State::kAccelerating) {
      acc_start_time_ = utils::Timer::getTimeMicros();
      // Generate a random time for a failure
      if (is_fail_) failure_time_ = (rand() % 20 + 1) * 1000000;
      acc_started_ = true;
    }
  }
  checkFailure();
  return local_data_;
}

void FakeBatteries::checkFailure()
{
  if (is_fail_ && failure_time_ != 0 && !failure_happened_) {
    if (utils::Timer::getTimeMicros() - acc_start_time_ >= failure_time_) {
      case_index_--;
      updateBatteryData();
      failure_happened_ = true;
    }
  }
}

void FakeBatteries::updateBatteryData()
{
  local_data_.voltage             = cases_[case_index_][0];
  local_data_.current             = cases_[case_index_][1];
  local_data_.charge              = cases_[case_index_][2];
  local_data_.low_temperature     = cases_[case_index_][3];
  local_data_.average_temperature = cases_[case_index_][4];
  local_data_.high_temperature    = cases_[case_index_][5];
  local_data_.low_voltage_cell    = cases_[case_index_][6];
  local_data_.high_voltage_cell   = cases_[case_index_][7];
  local_data_.imd_fault           = false;
}

bool FakeBatteries::isOnline()
{
  return true;
}

}  // namespace hyped::sensors
