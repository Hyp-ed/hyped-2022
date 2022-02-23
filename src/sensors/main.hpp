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
  using KeyencePins = std::array<uint32_t, data::Sensors::kNumKeyence>;
  using ImuPins     = std::array<uint32_t, data::Sensors::kNumImus>;

  Main();
  void run() override;  // from thread

  static std::optional<KeyencePins> keyencePinsFromFile(utils::Logger &log,
                                                        const std::string &path);
  static std::optional<ImuPins> imuPinsFromFile(utils::Logger &log, const std::string &path);
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
  data::Batteries batteries_;
  data::CounterData stripe_counter_;

  std::array<std::unique_ptr<ICounter>, data::Sensors::kNumKeyence> keyences_;  // 0 L and 1 R
  std::unique_ptr<ImuManager> imu_manager_;
  std::unique_ptr<BmsManager> battery_manager_;
  std::unique_ptr<ITemperature> temperature_;
  std::unique_ptr<IPressure> pressure_;
  bool log_error_ = false;

  data::TemperatureData temperature_data_;
  data::PressureData pressure_data_;
};

}  // namespace hyped::sensors
