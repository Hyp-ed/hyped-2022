#pragma once

#include "interface.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/io/adc.hpp>
#include <utils/logger.hpp>
namespace hyped::sensors {

class BrakePressure : public IBrakePressure {
 public:
  /**
   * @brief Construct a new Brake Pressure object
   *
   * @param pin for specific ADC pin
   */
  BrakePressure(const uint8_t pin);
  ~BrakePressure() {}

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
   * @brief scale raw digital data to output in bars
   *
   * @param raw_value input voltage
   * @return int representation of pressure
   */
  static int8_t scaleData(uint8_t raw_value);

  /**
   * @brief ADC pin
   */
  utils::io::ADC pin_;
  utils::Logger log_;

  /**
   * @brief int from data structs
   */
  data::BrakePressureData pressure_data_;
};

}  // namespace hyped::sensors
