#pragma once

#include "bms_manager.hpp"
#include "imu_manager.hpp"
#include "interface.hpp"

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
  using KeyencePins       = std::array<uint32_t, data::Sensors::kNumKeyence>;
  using ImuPins           = std::array<uint32_t, data::Sensors::kNumImus>;
  using BrakePressurePins = std::array<uint32_t, data::Sensors::kNumBrakePressure>;
  struct AmbientPressurePins {
    uint8_t pressure_pin;
    uint8_t temperature_pin;
  };

  Main();
  void run() override;  // from thread

  static std::optional<KeyencePins> keyencePinsFromFile(utils::Logger &log,
                                                        const std::string &path);
  static std::optional<ImuPins> imuPinsFromFile(utils::Logger &log, const std::string &path);
  static std::optional<uint32_t> temperaturePinFromFile(utils::Logger &log,
                                                        const std::string &path);
  static std::optional<AmbientPressurePins> ambientPressurePinsFromFile(utils::Logger &log,
                                                                        const std::string &path);
  static std::optional<BrakePressurePins> brakePressurePinsFromFile(utils::Logger &log,
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
  void checkAmbientPressure();

  /**
   * @brief used to check the brake pressure every twenty times in the main loop,
   *        similar to temperature;
   */
  void checkBrakePressure();

  utils::System &sys_;
  data::Data &data_;

  // master data structures
  data::Sensors sensors_;
  data::Batteries batteries_;
  data::CounterData stripe_counter_;

  std::array<std::unique_ptr<ICounter>, data::Sensors::kNumKeyence> keyences_;  // 0 L and 1 R
  std::unique_ptr<ImuManager> imu_manager_;
  std::unique_ptr<BmsManager> battery_manager_;

  data::TemperatureData temperature_data_;
  std::unique_ptr<ITemperature> temperature_;

  std::unique_ptr<IAmbientPressure> ambient_pressure_;
  data::AmbientPressureData pressure_data_;

  std::array<std::unique_ptr<IBrakePressure>, data::Sensors::kNumBrakePressure> brake_pressures_;
  std::array<data::BrakePressureData, data::Sensors::kNumBrakePressure> brake_pressure_data_;
};

}  // namespace hyped::sensors
