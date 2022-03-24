#pragma once

#include "interface.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/io/adc.hpp>

namespace hyped::sensors {

class AmbientPressure : public IAmbientPressure {
 public:
  /**
   * @brief Construct a new AmbientPressure object
   * @param pin for specific ADC pin
   */
  AmbientPressure(const uint8_t pressure_pin, const uint8_t temperature_pin_);
  ~AmbientPressure() {}

  /**
   * @brief
   *
   * @return uint16_t to set to data struct in sensors main
   */
  uint16_t getData() override;

  /**
   * @brief one iteration of checking sensors
   *
   */
  void run() override;

 private:
  std::optional<uint16_t> scaleData(const uint32_t digital_pressure_value,
                                    const uint32_t digital_temperature_value);

  // WARNING, TODO: This sensor doesn't actually use an analog signal!!!!
  utils::io::ADC pressure_pin_;
  utils::io::ADC temperature_pin_;

  utils::Logger log_;

  /**
   * @brief int from data structs
   */
  data::AmbientPressureData pressure_data_;
};
}  // namespace hyped::sensors
