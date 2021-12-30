#pragma once

#include <data/data.hpp>

namespace hyped::sensors {

class SensorInterface {
 public:
  /**
   * @brief Check if sensor is responding, i.e. connected to the system
   * @return true - if sensor is online
   */
  virtual bool isOnline() = 0;
};

class ImuInterface : public SensorInterface {
 public:
  /**
   * @brief Get IMU data
   * @param imu - output pointer to be filled by this sensor
   */
  virtual void getData(data::ImuData &imu_data) = 0;
};

class GpioInterface : public SensorInterface {
 public:
  /**
   * @brief Get GPIO data
   * @param stripe_counter - output pointer
   */
  virtual void getData(data::StripeCounter &stripe_counter) = 0;
};

class BmsInterface : public SensorInterface {
 public:
  /**
   * @brief Get Battery data
   * @param battery - output pointer to be filled by this sensor
   */
  virtual void getData(data::BatteryData &battery_data) = 0;
};

class TemperatureInterface {
 public:
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
