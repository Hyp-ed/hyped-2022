#pragma once

#include <cstdint>

#include <data/data.hpp>
#include <utils/logger.hpp>

namespace hyped::sensors {

class ITemperature {
 public:
  /**
   * @brief empty virtual deconstructor for proper deletion of derived classes
   */
  virtual ~ITemperature() {}

  /**
   * @brief not a thread, checks temperature
   */
  virtual void run() = 0;

  /**
   * @brief returns int representation of temperature
   * @return int temperature degrees C
   */
  virtual int getData() = 0;
};

class Temperature : public ITemperature {
 public:
  /**
   * @brief Construct a new Temperature object
   *
   * @param log from main thread, for debugging purposes
   * @param pin for specific ADC pin
   */
  Temperature(utils::Logger &log, int pin);
  ~Temperature() {}

  /**
   * @brief
   *
   * @return int to set to data struct in sensors main
   */
  int getData() override;

  /**
   * @brief one interation of checking sensors
   */
  void run() override;

 private:
  /**
   * @brief scale raw digital data to output in degrees C
   *
   * @param raw_value input voltage
   * @return int representation of temperature
   */
  int scaleData(uint16_t raw_value);

  /**
   * @brief ADC pin
   */
  int pin_;
  utils::Logger &log_;

  /**
   * @brief int from data structs
   */
  data::TemperatureData temp_;
};

}  // namespace hyped::sensors
