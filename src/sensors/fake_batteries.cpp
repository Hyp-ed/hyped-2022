#include "fake_batteries.hpp"

#include <stdlib.h>

#include <utils/timer.hpp>

namespace hyped {

namespace sensors {

FakeBatteries::FakeBatteries(Logger &log, bool is_lp, bool is_fail)
    : data_(Data::getInstance()),
      log_(log),
      lp_failure_{{100, 1000, 0, 0, 100, 0, 0, 0}},
      lp_success_{{35, 35, 35, 35, 35, 35, 35}},
      hp_failure_{{900, 10000, 0, 5, 70, 100, 500, 400}},
      hp_success_{{35, 35, 35, 35, 35, 35, 35}},
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
      log_.INFO("Fake-Batteries", "Fake LPBattery Fail initialised");
    } else {
      log_.INFO("Fake-Batteries", "Fake LPBattery initialised");
    }
  } else {
    if (is_fail_) {
      log_.INFO("Fake-Batteries", "Fake HPBattery Fail initialised");
    } else {
      log_.INFO("Fake-Batteries", "Fake HPBattery initialised");
    }
  }
}

void FakeBatteries::getData(BatteryData *battery)
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
  battery->voltage             = local_data_.voltage;
  battery->current             = local_data_.current;
  battery->charge              = local_data_.charge;
  battery->low_temperature     = local_data_.low_temperature;  // 0 for LP
  battery->average_temperature = local_data_.average_temperature;
  battery->high_temperature    = local_data_.high_temperature;   // 0 for LP
  battery->low_voltage_cell    = local_data_.low_voltage_cell;   // 0 for LP
  battery->high_voltage_cell   = local_data_.high_voltage_cell;  // 0 for LP
  battery->imd_fault           = local_data_.imd_fault;
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

}  // namespace sensors
}  // namespace hyped
