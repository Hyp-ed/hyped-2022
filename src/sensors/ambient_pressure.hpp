#pragma once

#include <cstdint>

#include <data/data.hpp>
#include <utils/io/adc.hpp>

namespace hyped::sensors {

struct AmbientPressurePins {
  uint8_t pressure_pin;
  uint8_t temperature_pin;
};

class IAmbientPressure {
 public:
  virtual uint16_t getData() const = 0;
  virtual void run()               = 0;
  virtual ~IAmbientPressure() {}
};

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
  uint16_t getData() const override;

  /**
   * @brief one iteration of checking sensors
   *
   */
  void run() override;

 private:
  std::optional<uint16_t> scaleData(const uint32_t digital_pressure_value,
                                    const uint32_t digital_temperature_value);

  // WARNING, TODO: This sensor doesn't actually use an analog signal!!!!
  utils::io::Adc pressure_pin_;
  utils::io::Adc temperature_pin_;

  utils::Logger log_;

  /**
   * @brief int from data structs
   */
  data::AmbientPressureData pressure_data_;
};
}  // namespace hyped::sensors
