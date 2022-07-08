#include "temperature.hpp"

#include <stdio.h>

#include <utils/system.hpp>

namespace hyped::sensors {

AmbientTemperature::AmbientTemperature(const uint8_t pin)
    : log_("AMBIENT TEMPERATURE", utils::System::getSystem().config_.log_level_sensors),
      pin_(pin)
{
  log_.info("started temperature for pin %u", pin);
}

AmbientTemperature::~AmbientTemperature()
{
  log_.info("stopped ambient temperature for pin");
}

void AmbientTemperature::run()
{
  uint16_t raw_value = pin_.read();
  log_.debug("raw value: %d", raw_value);
  temperature_data_.temperature = scaleData(raw_value);
  log_.debug("scaled value: %d", temperature_data_.temperature);
  temperature_data_.operational = true;
}

int8_t AmbientTemperature::scaleData(const uint8_t raw_value)
{
  // convert to degrees C
  double temp = static_cast<double>(raw_value) / 4095;
  temp        = (temp * 175) - 50;
  return static_cast<int8_t>(temp);
}

uint8_t AmbientTemperature::getData() const
{
  return temperature_data_.temperature;
}

BrakesAndSuspensionTemperature::BrakesAndSuspensionTemperature(const uint8_t pin)
    : log_("BRAKE-SUSP TEMPERATURE", utils::System::getSystem().config_.log_level_sensors),
      pin_(pin)
{
  log_.info("started temperature for pin %u", pin);
}

BrakesAndSuspensionTemperature::~BrakesAndSuspensionTemperature()
{
  log_.info("stopped brake/suspension temperature for pin");
}

void BrakesAndSuspensionTemperature::run()
{
  uint16_t raw_value = pin_.read();
  log_.debug("raw value: %d", raw_value);
  temperature_data_.temperature = scaleData(raw_value);
  log_.debug("scaled value: %d", temperature_data_.temperature);
  temperature_data_.operational = true;
}

int8_t BrakesAndSuspensionTemperature::scaleData(const uint8_t raw_value)
{
  // convert to degrees C
  double temp = static_cast<double>(raw_value) / 4095;
  temp        = (temp * 175) - 50;
  return static_cast<int8_t>(temp);
}

uint8_t BrakesAndSuspensionTemperature::getData() const
{
  return temperature_data_.temperature;
}
}  // namespace hyped::sensors
