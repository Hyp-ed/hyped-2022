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
  // calulation is linked by
  // https://protosupplies.com/product/lm35-analog-temp-sensor/#:~:text=The%20program%20uses%20the%20line%20float%20voltage%20%3D,10-bit%20ADC%20which%20gives%201024%20steps%20of%20resolution.
  // multiples by 5 bc the ADC reference is 5V, then divides by 1024 as the Ardunio has a 12-bit ADC
  // multiply this by 100 to get from voltage to celcius

  static constexpr int kValues[3] = {5, 4096, 100};

  // calculate the temp in C based off of raw readings
  //(raw_value * 5/4096) * 100 - issue right now is this 5!
  uint8_t temp = static_cast<int>((raw_value * kValues[0] / kValues[1]) * kValues[2]);

  return temp;
}

uint8_t Temperature::getData()
{
  return temp_.temp;
}
}  // namespace hyped::sensors
