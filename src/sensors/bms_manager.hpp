#pragma once

#include "bms.hpp"

#include <cstdint>
#include <memory>

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped::sensors {

class BmsManager : public utils::concurrent::Thread {
 public:
  struct Config {
    uint64_t bms_startup_time_micros;
  };
  void run() override;
  explicit BmsManager(const Config &config);
  static std::unique_ptr<BmsManager> fromFile(const std::string &path);

 private:
  std::array<std::unique_ptr<IBms>, data::FullBatteryData::kNumLPBatteries> low_power_batteries_;
  std::array<std::unique_ptr<IBms>, data::FullBatteryData::kNumHPBatteries> high_power_batteries_;
  utils::System &sys_;
  data::Data &data_;
  const Config config_;

  /**
   * @brief check IMD and set GPIOs accordingly
   */
  bool checkInuslationMonitoringDevice();

  /**
   * @brief holds LP BatteryData, HP BatteryData, and module_status
   */
  data::FullBatteryData battery_data_;

  /**
   * @brief print log messages once
   */
  data::ModuleStatus previous_status_;

  /**
   * @brief do not check ranges for first 5 seconds
   */
  uint64_t start_time_;

  /**
   * @brief checks voltage, current, temperature, and charge
   */
  bool checkBatteriesInRange();

  static std::optional<Config> readConfig(utils::Logger &log, const std::string &path);
};

}  // namespace hyped::sensors
