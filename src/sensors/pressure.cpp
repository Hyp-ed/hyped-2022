#include "pressure.hpp"

#include <stdio.h>

#include <utils/io/adc.hpp>
#include <utils/system.hpp>

namespace hyped::sensors {

Pressure::Pressure(const uint8_t pressure_pin, const uint8_t temp_pin)
    : pressure_pin_(pressure_pin),
      temp_pin_(temp_pin)
{
}

void Pressure::run()
{
  utils::io::ADC temppin(temp_pin_);
  utils::io::ADC pressurepin(pressure_pin_);
  utils::Logger log("PRESSURE", utils::System::getSystem().config_.log_level_sensors);
  pressure_data_.pressure    = 0;
  uint8_t raw_temp_value     = temppin.read();
  uint8_t raw_pressure_value = pressurepin.read();
  log.debug("Raw Pressure Data: %d", raw_temp_value, raw_pressure_value);
  pressure_data_.pressure = scaleData(raw_temp_value, raw_pressure_value);
  log.debug("Scaled Pressure Data: %d", pressure_data_.pressure);
  pressure_data_.operational = true;
}

uint8_t Pressure::scaleData(uint8_t raw_temp_value, uint8_t raw_pressure_value)
{
  // defines the constant factory coefficients
  static constexpr float kCoefficients[5] = {46372, 43981, 29059, 27842, 31553};

  // constant integer values used within the conversion formula
  static constexpr int kValues[6] = {256, 1024L, 16384L, 32768L, 2048L, 100};

  // method for conversion follows a method by David Edwards on the
  // datasheet link here:
  // https://www.alldatasheet.com/datasheet-pdf/pdf/880801/TEC/MS5607-02BA03.html
  // calibrates the raw values using the factory coefficients.

  const float delta_temp = raw_temp_value - kCoefficients[4] * kValues[0];
  const float P1         = ((raw_pressure_value) / kValues[1]) * ((kCoefficients[0]) / kValues[1]);
  const float P2         = ((raw_pressure_value) / kValues[2]) * ((kCoefficients[2]) / kValues[2])
                   * ((delta_temp) / kValues[3]);
  const float P3   = ((kCoefficients[1]) * 4);
  const float P4   = ((kCoefficients[3]) / kValues[1]) * ((delta_temp) / kValues[4]);
  uint8_t pressure = static_cast<int>((P1 + P2 - P3 - P4) / kValues[5]);

  return pressure;
}

uint8_t Pressure::getData()
{
  return pressure_data_.pressure;
}
}  // namespace hyped::sensors
