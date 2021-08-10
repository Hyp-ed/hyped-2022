#pragma once

#include "interface.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/logger.hpp>

namespace hyped {

using utils::Logger;

namespace sensors {

class Temperature : public TemperatureInterface {
 public:
  /**
   * @brief Construct a new Temperature object
   *
   * @param log from main thread, for debugging purposes
   * @param pin for specific ADC pin
   */
  Temperature(utils::Logger &log, int pin);
  ~Temperature() {}

  /**
   * @brief
   *
   * @return int to set to data struct in sensors main
   */
  int getData() override;

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
  int scaleData(uint16_t raw_value);

  /**
   * @brief ADC pin
   */
  int pin_;
  utils::Logger &log_;

  /**
   * @brief int from data structs
   */
  data::TemperatureData temp_;
};

}  // namespace sensors
}  // namespace hyped
