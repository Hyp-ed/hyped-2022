#pragma once

#include "interface.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/io/adc.hpp>
#include <utils/logger.hpp>
namespace hyped::sensors {

class Temperature_susp : public ITemperature_susp {
 public:
  /**
   * @brief Construct a new Temperature_susp object
   *
   * @param pin for specific ADC pin
   */
  Temperature_susp(const uint8_t pin);
  ~Temperature_susp() {}

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
   * @return int representation of temperature_susp
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
  data::Temperature_suspData temperature_susp_data_;
};

}  // namespace hyped::sensors
