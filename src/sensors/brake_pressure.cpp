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
  const uint16_t raw_value = pin_.read();
  log_.debug("raw value: %d", raw_value);
  pressure_data_.brake_pressure = scaleData(raw_value);
  log_.debug("scaled value: %d", pressure_data_.brake_pressure);
  pressure_data_.operational = true;
}

uint16_t BrakePressure::scaleData(const uint8_t raw_value)
{
  // TODO: Talk to Electronics to confirm the wiring diagram as scale changes based on that!!!!!!
  const float pressure = static_cast<float>(raw_value) / 4095;
  return static_cast<uint16_t>(pressure);
}

uint16_t BrakePressure::getData() const
{
  return pressure_data_.brake_pressure;
}
}  // namespace hyped::sensors
