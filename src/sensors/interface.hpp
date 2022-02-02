#pragma once

#include <string>

#include <data/data.hpp>

namespace hyped {
using data::BatteryData;
using data::ImuData;
using data::NavigationVector;
using data::PressureData;
using data::StripeCounter;
using data::TemperatureData;

namespace sensors {

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
  virtual void getData(ImuData *imu) = 0;
};

class GpioInterface : public SensorInterface {
 public:
  /**
   * @brief Get GPIO data
   * @param stripe_counter - output pointer
   */
  virtual void getData(StripeCounter *stripe_counter) = 0;
};

class BMSInterface : public SensorInterface {
 public:
  /**
   * @brief Get Battery data
   * @param battery - output pointer to be filled by this sensor
   */
  virtual void getData(BatteryData *battery) = 0;
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
  virtual uint8_t getData() = 0;
};

class PressureInterface {
 public:
  /**
   * @brief checks pressure
   * @return int pressure bars
   */
  virtual void run() = 0;

  /**
   * @brief returns int representation of pressure
   * @return int pressure bars
   */
  virtual int getData() = 0;
};
}  // namespace sensors
}  // namespace hyped
