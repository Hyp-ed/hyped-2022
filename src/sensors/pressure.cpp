#include "pressure.hpp"

#include <stdio.h>

#include <utils/io/adc.hpp>

namespace hyped::sensors {

Pressure::Pressure(hyped::utils::Logger &log, uint8_t pressure_pin, uint8_t temp_pin)
    : pressure_pin_(pressure_pin),
      temp_pin_(temp_pin),
      log_(log)
{
}

void Pressure::run()
{
  utils::io::ADC temppin(temp_pin_);
  utils::io::ADC pressurepin(pressure_pin_);
  pressure_.pressure         = 0;
  uint8_t raw_temp_value     = temppin.read();
  uint8_t raw_pressure_value = pressurepin.read();
  log_.DBG3("PRESSURE", "Raw Data: %d", raw_temp_value, raw_pressure_value);
  pressure_.pressure = scaleData(raw_temp_value, raw_pressure_value);
  log_.DBG3("PRESSURE", "Scaled Data: %d", pressure_.pressure);
  pressure_.operational = true;
}

int Pressure::scaleData(uint8_t raw_temp_value, uint8_t raw_pressure_value)
{
  // define the constant factory coefficients
  const int coefficients_[5] = {46372, 43981, 29059, 27842, 31553};

  // method for conversion follows a method by David Edwards
  // calibrates the raw values using the factory coefficients.
  float delta_temp = raw_temp_value - coefficients_[4] * 256;
  float P1         = (static_cast<float>(raw_pressure_value) / 1024L)
             * (static_cast<float>(coefficients_[0]) / 1024L);
  float P2 = (static_cast<float>(raw_pressure_value) / 16384L)
             * (static_cast<float>(coefficients_[2]) / 16384L)
             * (static_cast<float>(delta_temp) / 32768L);
  float P3 = (static_cast<float>(coefficients_[1]) * 4);
  float P4
    = (static_cast<float>(coefficients_[3]) / 1024l) * (static_cast<float>(delta_temp) / 2048L);
  int pressure = static_cast<int>(P1 + P2 - P3 - P4);

  return pressure;
}

int Pressure::getData()
{
  return pressure_.pressure;
}
}  // namespace hyped::sensors
