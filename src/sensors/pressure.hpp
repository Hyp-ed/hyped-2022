#pragma once

#include "interface.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/logger.hpp>

namespace hyped::sensors
{

  class Pressure : public PressureInterface
  {
  public:
    /**
   * @brief Construct a new Pressure object
   *
   * @param log from main thread, for debugging purposes
   * @param pin for specific ADC pin
   */
    Pressure(utils::Logger &log, int pin);

    /**
   * @brief
   *
   * @return int to set to data struct in sensors main
   */
    int getData() override;

    /**
   * @brief one iteration of checking sensors
   *
   */
    void run() override;

  private:
    /**
   * @brief scale raw difital data to output in ms??
   *
   * @param raw_value input voltage
   * @return float representation of pressure
   */
    double scaleData(uint8_t raw_value);

    /**
   * @brief ADC pin
   */
    int pin_;
    utils::Logger &log_;

    /**
   * @brief int from data structs
   */
    data::PressureData pressure_;
  };
} // namespace hyped::sensors
