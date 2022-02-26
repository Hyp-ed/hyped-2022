#pragma once

#include "interface.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/logger.hpp>

namespace hyped::sensors {

class Pressure : public IPressure {
 public:
  /**
   * @brief Construct a new Pressure object
   * @param pin for specific ADC pin
   */
  Pressure(uint8_t pressure_pin, uint8_t temp_pin);
  ~Pressure() {}

  /**
   * @brief
   *
   * @return uint16_t to set to data struct in sensors main
   */
  uint8_t getData() override;

  /**
   * @brief one iteration of checking sensors
   *
   */
  void run() override;

 private:
  /**
   * @brief scale raw digital data to output in ms
   *
   * @param raw_temp_value input temperature voltage
   * @param raw_pressure_value input pressure voltage
   * @return int representation of pressure
   */
  uint8_t scaleData(uint8_t raw_temp_value, uint8_t raw_pressure_value);

  /**
   * @brief ADC pin
   */
  int pressure_pin_;
  int temp_pin_;

  /**
   * @brief int from data structs
   */
  data::PressureData pressure_data_;
};
}  // namespace hyped::sensors
