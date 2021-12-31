#pragma once

#include "interface.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped {

using hyped::data::BatteryData;
using utils::Logger;
using utils::concurrent::Thread;

namespace sensors {

class BmsManager : public Thread {
 public:
  explicit BmsManager(Logger &log);
  void run() override;

 private:
  IBms *bms_[data::Batteries::kNumLPBatteries + data::Batteries::kNumHPBatteries];
  utils::System &sys_;

  /**
   * @brief check IMD and set GPIOs accordingly
   */
  bool checkIMD();

  /**
   * @brief needs to be references because run() passes directly to data struct
   */
  data::Data &data_;

  /**
   * @brief holds LP BatteryData, HP BatteryData, and module_status
   */
  data::Batteries batteries_;

  /**
   * @brief print log messages once
   */
  data::ModuleStatus previous_status_;

  /**
   * @brief do not check ranges for first 5 seconds
   */
  uint64_t start_time_;
  uint64_t check_time_ = 5000000;

  /**
   * @brief checks voltage, current, temperature, and charge
   */
  bool batteriesInRange();
};

}  // namespace sensors
}  // namespace hyped
