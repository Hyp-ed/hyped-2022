#pragma once

#include "bms_manager.hpp"
#include "imu_manager.hpp"
#include "interface.hpp"

#include <cstdint>

#include <utils/system.hpp>

namespace hyped {

namespace sensors {

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
   * @brief as long as at least one keyence value is updated
   *
   * @return true
   * @return false
   */
  bool keyencesUpdated();

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

  data::Data &data_;
  utils::System &sys_;
  utils::Logger &log_;

  // master data structures
  data::Sensors sensors_;
  data::Batteries batteries_;
  data::StripeCounter stripe_counter_;

  uint8_t pins_[data::Sensors::kNumKeyence];
  GpioInterface *keyences_[data::Sensors::kNumKeyence];  // 0 L and 1 R
  ImuManager *imu_manager_;
  BmsManager *battery_manager_;
  TemperatureInterface *temperature_;
  bool log_error_ = false;

  /**
   * @brief update this from GpioCounter::getStripeCounter();
   */
  array<data::StripeCounter, data::Sensors::kNumKeyence> keyence_stripe_counter_arr_;

  /**
   * @brief use this to compare with keyence_stripe_counter_arr_
   *        update when keyenceUpdated() == true
   */
  array<data::StripeCounter, data::Sensors::kNumKeyence> prev_keyence_stripe_count_arr_;

  /**
   * @brief use this to take the temperature so far
   *
   */

  TemperatureData temp_;

  uint8_t converted_temp_;

  // Sensors sensors_data_;
};

}  // namespace sensors
}  // namespace hyped
