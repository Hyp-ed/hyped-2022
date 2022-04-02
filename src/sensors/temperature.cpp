#include "temperature.hpp"

#include <stdio.h>

#include <utils/system.hpp>

namespace hyped::sensors {

Temperature::Temperature(const uint8_t pin)
    : log_("TEMPERATURE", utils::System::getSystem().config_.log_level_sensors),
      pin_(pin)
{
  log_.info("started temperature for pin %u", pin);
}

Temperature::~Temperature()
{
  log_.info("stopped temperature for pin");
}

void Temperature::run()
{
  uint16_t raw_value = pin_.read();
  log_.debug("raw value: %d", raw_value);
  temperature_data_.temperature = scaleData(raw_value);
  log_.debug("scaled value: %d", temperature_data_.temperature);
  temperature_data_.operational = true;
}

int8_t Temperature::scaleData(const uint8_t raw_value)
{
  // convert to degrees C
  double temp = static_cast<double>(raw_value) / 4095;
  temp        = (temp * 175) - 50;
  return static_cast<int8_t>(temp);
}

uint8_t Temperature::getData() const
{
  return temperature_data_.temperature;
}
}  // namespace hyped::sensors
