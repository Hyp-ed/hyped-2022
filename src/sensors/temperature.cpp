#include "temperature.hpp"

#include <stdio.h>

#include <utils/io/adc.hpp>

namespace hyped {

using data::Data;
using data::TemperatureData;
using hyped::utils::Logger;
using utils::io::ADC;

namespace sensors {

Temperature::Temperature(utils::Logger &log, int pin) : pin_(pin), log_(log)
{
}

void Temperature::run()
{
  ADC thepin(pin_);
  temp_.temp         = 0;
  uint16_t raw_value = thepin.read();
  log_.debug("Raw Data: %d", raw_value);
  temp_.temp = scaleData(raw_value);
  log_.debug("Scaled Data: %d", temp_.temp);
  temp_.operational = true;
}

uint8_t Temperature::scaleData(uint8_t raw_value)
{
  // convert to degrees C
  double temp = static_cast<double>(raw_value) / 4095;
  temp        = (temp * 175) - 50;
  return static_cast<uint8_t>(temp);
}

uint8_t Temperature::getData()
{
  return temp_.temp;
}
}  // namespace sensors
}  // namespace hyped
