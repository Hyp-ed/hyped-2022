#include "pressure.hpp"

#include <stdio.h>

#include <utils/io/adc.hpp>

namespace hyped::sensors {

Pressure::Pressure(hyped::utils::Logger &log, int pressure_pin, int temp_pin)
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

double Pressure::scaleData(uint8_t raw_temp_value, uint8_t raw_pressure_value)
{
  // define the constant factory coefficients
  const uint8_t coefficients_[5] = {46372, 43981, 29059, 27842, 31553};

  // method for conversion follows a method by David Edwards
  // calibrates the raw values using the factory coefficients.
  double delta_temp = raw_temp_value - coefficients_[4] * 256;
  double P1         = (float(raw_pressure_value) / 1024L) * (float(coefficients_[0]) / 1024L);
  double P2         = (float(raw_pressure_value) / 16384L) * (float(coefficients_[2]) / 16384L)
              * (float(delta_temp) / 32768L);
  double P3       = (float(coefficients_[1]) * 4);
  double P4       = (float(coefficients_[3]) / 1024l) * (float(delta_temp) / 2048L);
  double pressure = P1 + P2 - P3 - P4;

  return pressure;
}

int Pressure::getData()
{
  return pressure_.pressure;
}
}  // namespace hyped::sensors
