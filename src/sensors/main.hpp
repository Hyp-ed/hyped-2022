#pragma once

#include "bms_manager.hpp"
#include "imu_manager.hpp"
#include "interface.hpp"

#include <cstdint>

#include <utils/system.hpp>

namespace hyped::sensors {

/**
 * @brief Initialise sensors, data instances to be pulled in managers
 *        gpio threads and adc checks declared in main
 */
class Main : public Thread {
 public:
  Main(uint8_t id, utils::Logger &log);
  void run() override;  // from thread

 private:
  /**
   * @brief checks range of pod temperature
   *
   * @return true if status is valid
   * @return false if kCriticalFailure
   */
  bool temperatureInRange();

  /**
   * @brief used to check the temperature infrequently in main loop,
   *        unnecessary to constantly check temperature;
   */
  void checkTemperature();

  utils::System &sys_;
  data::Data &data_;
  utils::Logger &log_;

  // master data structures
  data::Sensors sensors_;
  data::Batteries batteries_;
  data::CounterData stripe_counter_;

  std::array<uint8_t, data::Sensors::kNumKeyence> keyence_pins_;
  std::array<std::unique_ptr<ICounter>, data::Sensors::kNumKeyence> keyences_;  // 0 L and 1 R
  std::unique_ptr<ImuManager> imu_manager_;
  std::unique_ptr<BmsManager> battery_manager_;
  std::unique_ptr<ITemperature> temperature_;
  bool log_error_ = false;
};

}  // namespace hyped::sensors
