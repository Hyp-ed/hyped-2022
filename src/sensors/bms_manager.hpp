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
  struct Config {
    uint64_t bms_timeout;
  };
  void run() override;
  static std::unique_ptr<BmsManager> fromFile(const std::string &path);

 private:
  IBms *bms_[data::Batteries::kNumLPBatteries + data::Batteries::kNumHPBatteries];
  utils::System &sys_;
  data::Data &data_;
  const Config config_;

  /**
   * @brief check IMD and set GPIOs accordingly
   */
  bool checkIMD();

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

  explicit BmsManager(utils::Logger log, const Config &config);
};

}  // namespace sensors
}  // namespace hyped
