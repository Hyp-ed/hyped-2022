#pragma once

#include "bms.hpp"

#include <vector>

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped {

namespace sensors {

class FakeBatteries : public IBms {
  using BatteryInformation = std::array<uint16_t, 8>;
  using BatteryCases       = std::array<BatteryInformation, 4>;

 public:
  /**
   * @brief Construct a new Fake Batteries object
   *
   * @param log
   * @param is_lp
   * @param is_fail
   */
  FakeBatteries(utils::Logger &log, bool is_lp, bool is_fail);

  /**
   * @brief waits for accelerating state, generate random time for error
   * @param battery BatteryData pointer
   */
  data::BatteryData getData() override;
  bool isOnline() override;

 private:
  data::Data &data_;
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

  data::BatteryData local_data_;

  uint64_t acc_start_time_;
  bool acc_started_;
  uint64_t failure_time_;
  bool failure_happened_;
};
}  // namespace sensors
}  // namespace hyped
