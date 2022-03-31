#pragma once

#include <data/data.hpp>

namespace hyped::sensors {

class ISensor {
 public:
  /**
   * @brief Check if sensor is responding, i.e. connected to the system
   * @return true - if sensor is online
   */
  virtual bool isOnline() = 0;
};

class IImu : public ISensor {
 public:
  /**
   * @brief empty virtual deconstructor for proper deletion of derived classes
   */
  virtual ~IImu() {}

  /**
   * @brief Get IMU data
   * @param imu - output pointer to be filled by this sensor
   */
  virtual data::ImuData getData() = 0;
};

class ICounter : public ISensor {
 public:
  /**
   * @brief empty virtual deconstructor for proper deletion of derived classes
   */
  virtual ~ICounter() {}

  /**
   * @brief Get GPIO data
   * @param stripe_counter - output pointer
   */
  virtual data::CounterData getData() = 0;
};

class IBms : public ISensor {
 public:
  /**
   * @brief empty virtual deconstructor for proper deletion of derived classes
   */
  virtual ~IBms() {}

  /**
   * @brief Get Battery data
   * @param battery - output pointer to be filled by this sensor
   */
  virtual data::BatteryData getData() = 0;
};

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
  virtual uint8_t getData() const = 0;
};

class IAmbientPressure {
 public:
  /**
   * @brief empty virtual deconstructor for proper deletion of derived classes
   */
  virtual ~IAmbientPressure() {}

  /**
   * @brief checks pressure
   */
  virtual void run() = 0;

  /**
   * @return pressure in mbars
   */
  virtual uint16_t getData() const = 0;
};
}  // namespace hyped::sensors
