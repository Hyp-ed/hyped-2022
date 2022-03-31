#include "brake_pressure.hpp"

#include <stdio.h>

#include <utils/system.hpp>

namespace hyped::sensors {

BrakePressure::BrakePressure(const uint8_t pin)
    : pin_(pin),
      log_("BRAKE-PRESSURE", utils::System::getSystem().config_.log_level_sensors)
{
}

void BrakePressure::run()
{
  uint16_t raw_value = pin_.read();
  log_.debug("raw value: %d", raw_value);
  pressure_data_.brake_pressure = scaleData(raw_value);
  log_.debug("scaled value: %d", pressure_data_.brake_pressure);
  pressure_data_.operational = true;
}

int8_t BrakePressure::scaleData(const uint8_t raw_value)
{
  // TODO: Talk to Electronics to confirm the wiring diagram as scale changes based on that
  // DO NOT MERGE UNTIL THIS IS FIXED
  double pressure = static_cast<double>(raw_value) / 4095;
  return static_cast<int8_t>(pressure);
}

uint8_t BrakePressure::getData() const
{
  return pressure_data_.brake_pressure;
}
}  // namespace hyped::sensors
