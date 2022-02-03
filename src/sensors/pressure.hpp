#pragma once

#include "interface.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/logger.hpp>

namespace hyped::sensors {

class Pressure : public PressureInterface {
 public:
  /**
   * @brief Construct a new Pressure object
   *
   * @param log from main thread, for debugging purposes
   * @param pin for specific ADC pin
   */
  Pressure(utils::Logger &log, uint8_t pressure_pin, uint8_t temp_pin);

  /**
   * @brief
   *
   * @return int to set to data struct in sensors main
   */
  int getData() override;

  /**
   * @brief one iteration of checking sensors
   *
   */
  void run() override;

 private:
  /**
   * @brief scale raw difital data to output in ms??
   *
   * @param raw_temp_value input temperature voltage
   * @param raw_pressure_value input pressure voltage
   * @return int representation of pressure
   */
  int scaleData(uint8_t raw_temp_value, uint8_t raw_pressure_value);

  /**
   * @brief ADC pin
   */
  int pressure_pin_;
  int temp_pin_;
  utils::Logger &log_;

  /**
   * @brief int from data structs
   */
  data::PressureData pressure_;
};
}  // namespace hyped::sensors
