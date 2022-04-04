#include "ambient_pressure.hpp"

#include <stdio.h>

#include <utils/system.hpp>

namespace hyped::sensors {

AmbientPressure::AmbientPressure(const uint8_t pressure_pin, const uint8_t temperature_pin)
    : pressure_pin_(pressure_pin),
      temperature_pin_(temperature_pin),
      log_("AMBIENT-PRESSURE", utils::System::getSystem().config_.log_level_sensors)
{
}

void AmbientPressure::run()
{
  pressure_data_.ambient_pressure          = 0;
  const uint16_t digital_temperature_value = temperature_pin_.read();
  const uint16_t digital_pressure_value    = pressure_pin_.read();
  log_.debug("Raw AmbientPressure Data: %d", digital_temperature_value, digital_pressure_value);
  const auto scaled_data = scaleData(digital_pressure_value, digital_temperature_value);
  if (!scaled_data) {
    pressure_data_.operational = false;
    return;
  }
  pressure_data_.ambient_pressure = *scaled_data;
  log_.debug("Scaled AmbientPressure Data: %d", pressure_data_.ambient_pressure);
  pressure_data_.operational = true;
}

std::optional<uint16_t> AmbientPressure::scaleData(const uint32_t digital_pressure_value,     // D1
                                                   const uint32_t digital_temperature_value)  // D2
{
  if (digital_pressure_value > 1 << 24) {
    log_.error("Digital ambient_pressure value (%u) exceeds maximum. (%u)", digital_pressure_value,
               1 << 24);
    return std::nullopt;
  }
  if (digital_temperature_value > 1 << 24) {
    log_.error("Digital temperature value (%u) exceeds maximum. (%u)", digital_temperature_value,
               1 << 24);
    return std::nullopt;
  }
  static constexpr uint16_t kPressureSensitivity                         = 46372;  // C1
  static constexpr uint16_t kPressureOffset                              = 43981;  // C2
  static constexpr uint16_t kTemperatureCoefficientOfPressureSensitivity = 29059;  // C3
  static constexpr uint16_t kTemperatureCoefficientOfPressureOffset      = 27842;  // C4
  static constexpr uint16_t kReferenceTemperature                        = 31553;  // C5

  // method for conversion follows a method by David Edwards on the
  // datasheet link here:
  // https://www.alldatasheet.com/datasheet-pdf/pdf/880801/TEC/MS5607-02BA03.html
  // calibrates the raw values using the factory coefficients.

  // CALCULATE TEMPERATURE (we only need the difference)

  // dT = D2 - C5 * 2^8
  const int32_t temperature_difference = digital_temperature_value - (kReferenceTemperature << 8);
  if (temperature_difference < -16776960 || temperature_difference > 16777216) {
    log_.error("Temperature difference (%d) exceeds bounds. (%d to %d)", temperature_difference,
               -16776960, 16777216);
    return std::nullopt;
  }

  // CALCULATE TEMPERATURE COMPENSATED PRESSURE

  // OFF = C2 * 2^17 + (C4 * dT) / 2^6
  const int64_t offset_at_actual_temperature
    = (static_cast<int64_t>(kPressureOffset) << 17)
      + ((kTemperatureCoefficientOfPressureOffset * temperature_difference) >> 6);
  if (offset_at_actual_temperature < -17179344900 || offset_at_actual_temperature > 25769410560) {
    log_.error("Offset at actual temperature (%d) exceeds bounds. (%d to %d)",
               offset_at_actual_temperature, -17179344900, 25769410560);
    return std::nullopt;
  }

  // SENS = C1 * 2^16 + (C3 * dT) / 2^7
  const int64_t sensitivity_at_actual_temperature
    = (kPressureSensitivity << 16)
      + ((kTemperatureCoefficientOfPressureSensitivity * temperature_difference) >> 7);
  if (sensitivity_at_actual_temperature < -8589672450
      || sensitivity_at_actual_temperature > 12884705280) {
    log_.error("Sensitivity at actual temperature (%d) exceeds bounds. (%d to %d)",
               sensitivity_at_actual_temperature, -8589672450, 12884705280);
    return std::nullopt;
  }

  // P = (D1 * SENS / 2^21 - OFF) / 2^15
  const int32_t actual_pressure
    = (((digital_pressure_value * sensitivity_at_actual_temperature) >> 21)
       - offset_at_actual_temperature)
      >> 15;
  if (actual_pressure < 1000 || actual_pressure > 120000) {
    log_.error("Compensated ambient_pressure value (%d) out of bounds. (%d to %d)", actual_pressure,
               1000, 120000);
    return std::nullopt;
  }
  return static_cast<uint8_t>(actual_pressure / 100);  // mbar
}

uint16_t AmbientPressure::getData() const
{
  return pressure_data_.ambient_pressure;
}
}  // namespace hyped::sensors
