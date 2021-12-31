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
  virtual void getData(data::ImuData &imu_data) = 0;
};

class IGpio : public ISensor {
 public:
  /**
   * @brief empty virtual deconstructor for proper deletion of derived classes
   */
  virtual ~IGpio() {}

  /**
   * @brief Get GPIO data
   * @param stripe_counter - output pointer
   */
  virtual void getData(data::StripeCounter &stripe_counter) = 0;
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
  virtual void getData(data::BatteryData &battery_data) = 0;
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
  virtual int getData() = 0;
};
}  // namespace hyped::sensors
