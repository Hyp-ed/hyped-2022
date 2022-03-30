#pragma once

#include "bms_manager.hpp"
#include "imu_manager.hpp"
#include "pressure.hpp"
#include "sensor.hpp"
#include "temperature.hpp"

#include <cstdint>
#include <memory>
#include <string>

#include <utils/system.hpp>

namespace hyped::sensors {

using ImuPins = std::array<uint32_t, data::Sensors::kNumImus>;

/**
 * @brief Initialise sensors, data instances to be pulled in managers
 *        gpio threads and adc checks declared in main
 */
class Main : public utils::concurrent::Thread {
 public:
  Main();
  void run() override;  // from thread

  static std::optional<std::vector<uint8_t>> imuPinsFromFile(utils::Logger &log,
                                                             const std::string &path);
  static std::optional<uint32_t> temperaturePinFromFile(utils::Logger &log,
                                                        const std::string &path);

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

  /**
   * @brief used to check the pressure every twenty times in the main loop,
   *        similar to temperature;
   */
  void checkPressure();

  utils::System &sys_;
  data::Data &data_;

  // master data structures
  data::Sensors sensors_;
  data::FullBatteryData batteries_;
  data::CounterData stripe_counter_;

  std::unique_ptr<ImuManager> imu_manager_;
  std::unique_ptr<BmsManager> battery_manager_;
  std::unique_ptr<ITemperature> temperature_;
  std::unique_ptr<IPressure> pressure_;
  bool log_error_ = false;

  data::TemperatureData temperature_data_;
  data::PressureData pressure_data_;
};

}  // namespace hyped::sensors
