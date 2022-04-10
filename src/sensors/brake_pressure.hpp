#pragma once

#include <cstdint>

#include <data/data.hpp>
#include <utils/io/adc.hpp>
#include <utils/logger.hpp>
namespace hyped::sensors {

class IBrakePressure {
 public:
  virtual uint16_t getData() const = 0;
  virtual void run()               = 0;
  virtual ~IBrakePressure() {}
};
class BrakePressure : public IBrakePressure {
 public:
  /**
   * @brief Construct a new Brake Pressure object
   *
   * @param pin for specific ADC pin
   */
  BrakePressure(const uint8_t pin);
  ~BrakePressure() {}

  uint16_t getData() const override;

  /**
   * @brief one interation of checking sensors
   */
  void run() override;

 private:
  /**
   * @brief scale raw digital data to output in mBars
   *
   * @param raw_value input voltage
   * @return int representation of pressure
   */
  static uint16_t scaleData(const uint8_t raw_value);

  utils::io::Adc pin_;
  utils::Logger log_;

  data::BrakePressureData pressure_data_;
};

}  // namespace hyped::sensors
