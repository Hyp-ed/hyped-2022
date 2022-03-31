#pragma once

#include <cstdint>

#include <data/data.hpp>
#include <utils/io/adc.hpp>
#include <utils/logger.hpp>
namespace hyped::sensors {

using AmbientTemperaturePins = std::array<uint8_t, data::Sensors::kNumAmbientTemp>;
using BrakeTemperaturePins   = std::array<uint8_t, data::Sensors::kNumBrakeTemp>;

class ITemperature {
 public:
  /**
   * @brief empty virtual deconstructor for proper deletion of derived classes
   */
  virtual ~ITemperature() {}

  /**
   * @brief not a thread, checks temperature
   */
  virtual void run() = 0;

  /**
   * @brief returns int representation of temperature
   * @return int temperature degrees C
   */
  virtual uint8_t getData() const = 0;
};

class Temperature : public ITemperature {
 public:
  /**
   * @brief Construct a new Temperature object
   *
   * @param pin for specific ADC pin
   */
  Temperature(const uint8_t pin);
  ~Temperature();

  /**
   * @brief
   *
   * @return int to set to data struct in sensors main
   */
  uint8_t getData() const override;

  /**
   * @brief one interation of checking sensors
   */
  void run() override;

 private:
  /**
   * @brief scale raw digital data to output in degrees C
   *
   * @param raw_value input voltage
   * @return int representation of temperature
   */
  static int8_t scaleData(uint8_t raw_value);

  utils::Logger log_;
  utils::io::Adc pin_;

  /**
   * @brief int from data structs
   */
  data::TemperatureData temperature_data_;
};

}  // namespace hyped::sensors
