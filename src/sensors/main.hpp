#pragma once

#include "ambient_pressure.hpp"
#include "bms_manager.hpp"
#include "imu_manager.hpp"
#include "sensor.hpp"
#include "temperature.hpp"

#include <cstdint>
#include <memory>
#include <string>

#include <utils/system.hpp>

namespace hyped::sensors {

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
  static std::optional<std::vector<uint8_t>> ambientTemperaturePinsFromFile(
    utils::Logger &log, const std::string &path);
  static std::optional<std::vector<uint8_t>> brakeTemperaturePinsFromFile(utils::Logger &log,
                                                                          const std::string &path);
  static std::optional<AmbientPressurePins> ambientPressurePinsFromFile(utils::Logger &log,
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
   * @brief used to check the temperature of the ambient temperature sensors
   *        infrequently in main loop, unnecessary to constantly check temperature;
   */
  void checkAmbientTemperature();

  /**
   * @brief used to check the temperature of the brake temperature sensors
   *        infrequently in main loop, unnecessary to constantly check temperature;
   */
  void checkBrakeTemperature();

  /**
   * @brief used to check the pressure every twenty times in the main loop,
   *        similar to temperature;
   */
  void checkAmbientPressure();

  utils::System &sys_;
  data::Data &data_;

  // master data structures
  data::Sensors sensors_;
  data::FullBatteryData batteries_;
  data::CounterData stripe_counter_;

  std::unique_ptr<ImuManager> imu_manager_;
  std::unique_ptr<BmsManager> battery_manager_;

  std::array<std::unique_ptr<ITemperature>, data::Sensors::kNumAmbientTemp> ambient_temperatures_;
  std::array<std::unique_ptr<ITemperature>, data::Sensors::kNumBrakeTemp> brake_temperatures_;
  std::unique_ptr<IAmbientPressure> ambient_pressure_;

  data::TemperatureData temperature_data_;
  data::AmbientPressureData pressure_data_;
};

}  // namespace hyped::sensors
