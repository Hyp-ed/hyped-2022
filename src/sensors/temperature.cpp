#include "temperature.hpp"

#include <stdio.h>

#include <utils/io/adc.hpp>
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
  temp_.temp         = 0;
  uint16_t raw_value = pin_.read();
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
}  // namespace hyped::sensors
