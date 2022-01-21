#include "pressure.hpp"

#include <stdio.h>

#include <utils/io/adc.hpp>

namespace hyped
{

  using data::Data;
  using data::PressureData;
  using hyped::utils::Logger;
  using utils::io::ADC;

  namespace sensors
  {

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
      //TODO: convert voltages to bar
      //To convert ADC bins into volts
      double pressure = static_cast<double>(raw_value) / 4095;
    }

    int Pressure::getData()
    {
      return pressure_.pressure;
    }
  } //namespace sensors
} //namespace hyped
