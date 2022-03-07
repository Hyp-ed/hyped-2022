#pragma once

#include "interface.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/logger.hpp>

namespace hyped::sensors {

class Temperature : public ITemperature {
 public:
  /**
   * @brief Construct a new Temperature object
   *
   * @param pin for specific ADC pin
   */
  Temperature(uint8_t pin);
  ~Temperature() {}

  /**
   * @brief
   *
   * @return int to set to data struct in sensors main
   */
  uint8_t getData() override;

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
  uint8_t scaleData(uint8_t raw_value);

  /**
   * @brief ADC pin
   */
  uint8_t pin_;

  /**
   * @brief int from data structs
   */
  data::TemperatureData temp_;
};

}  // namespace hyped::sensors
