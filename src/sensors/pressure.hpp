#pragma once

#include <cstdint>

#include <data/data.hpp>

namespace hyped::sensors {

class IPressure {
 public:
  /**
   * @brief empty virtual deconstructor for proper deletion of derived classes
   */
  virtual ~IPressure() {}
  /**
   * @brief checks pressure
   */
  virtual void run() = 0;

  /**
   * @brief returns int representation of pressure
   * @return int pressure bars
   */
  virtual uint16_t getData() = 0;
};

class Pressure : public IPressure {
 public:
  /**
   * @brief Construct a new Pressure object
   * @param pin for specific ADC pin
   */
  Pressure(const uint8_t pressure_pin, const uint8_t temp_pin);
  ~Pressure() {}

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
