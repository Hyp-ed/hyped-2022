#pragma once

#include <string>
#include <utils/concurrent/thread.hpp>
#include <vector>

#include "interface.hpp"

namespace hyped {

using data::Data;
using utils::Logger;

namespace sensors {

class FakeBatteries : public BMSInterface {
  typedef array<uint16_t, 8> BatteryInformation;
  typedef array<BatteryInformation, 4> BatteryCases;

 public:
  /**
   * @brief Construct a new Fake Batteries object
   *
   * @param log
   * @param is_lp
   * @param is_fail
   */
  FakeBatteries(Logger &log, bool is_lp, bool is_fail);

  /**
   * @brief waits for accelerating state, generate random time for error
   * @param battery BatteryData pointer
   */
  void getData(BatteryData *battery) override;
  bool isOnline() override;

 private:
  Data &data_;
  utils::Logger &log_;

  /**
   * @brief if is_fail_ == true, will throw failure
   */
  void checkFailure();

  /**
   * @brief updates values from array given case_index_
   */
  void updateBatteryData();

  BatteryInformation lp_failure_;
  BatteryInformation lp_success_;
  BatteryInformation hp_failure_;
  BatteryInformation hp_success_;
  // different success and fail cases
  BatteryCases cases_;

  bool is_lp_;
  bool is_fail_;
  int case_index_;  // handle for array of values for both hp/lp

  BatteryData local_data_;

  uint64_t acc_start_time_;
  bool acc_started_;
  uint64_t failure_time_;
  bool failure_happened_;
};
}  // namespace sensors
}  // namespace hyped
