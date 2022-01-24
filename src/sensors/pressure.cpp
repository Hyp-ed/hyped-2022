#include "pressure.hpp"

#include <stdio.h>

#include <utils/io/adc.hpp>

namespace hyped {

using data::Data;
using data::PressureData;
using hyped::utils::Logger;
using utils::io::ADC;

namespace sensors {

Pressure::Pressure(utils::Logger &log, int pin) : pin_(pin), log_(log)
{
}

void Pressure::run()
{
  ADC thepin(pin_);
  pressure_.pressure = 0;
  uint16_t raw_value = thepin.read();
  log_.DBG3("PRESSURE", "Raw Data: %d", raw_value);
  pressure_.pressure = scaleData(raw_value);
  log_.DBG3("PRESSURE", "Scaled Data: %d", pressure_.pressure);
  pressure_.operational = true;
}

int Pressure::scaleData(uint16_t raw_value)
{
  // To convert ADC bins into volts
  double pressure = (static_cast<double>(raw_value) / 4095) * 5;

  // 0 - 5 volts to 0 - 10 equal multiply by 2.
  pressure = 2 * pressure;

  // truncate down
  return static_cast<int>(pressure);
}

int Pressure::getData()
{
  return pressure_.pressure;
}
}  // namespace sensors
}  // namespace hyped
