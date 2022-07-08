#include "temperature.hpp"

#include <stdio.h>

#include <utils/system.hpp>

namespace hyped::sensors {

Temperature_susp::Temperature_susp(const uint8_t pin)
    : pin_(pin),
      log_("TEMPERATURE", utils::System::getSystem().config_.log_level_sensors)
{
}

void Temperature_susp::run()
{
  uint16_t raw_value = pin_.read();
  log_.debug("raw value: %d", raw_value);
  temperature_susp_data_.temperature_susp = scaleData(raw_value);
  log_.debug("scaled value: %d", temperature_susp_data_.temperature_susp);
  temperature_susp_data_.operational = true;
}

int8_t Temperature_susp::scaleData(const uint8_t raw_value)
{
  // convert to degrees C
  double temp = static_cast<double>(raw_value) / 4095;
  temp        = ((temp * 175) - 50)/(4*20*0.001);
  return static_cast<int8_t>(temp);
}

uint8_t Temperature_susp::getData() const
{
  return temperature_data_.temperature;
}
}  // namespace hyped::sensors