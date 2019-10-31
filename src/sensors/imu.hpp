/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 23/05/18
 * Description: Main file for Imu
 *
 *    Copyright 2018 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef SENSORS_IMU_HPP_
#define SENSORS_IMU_HPP_

#include <vector>

#include "sensors/interface.hpp"
#include "utils/logger.hpp"
#include "utils/io/spi.hpp"
#include "utils/io/gpio.hpp"

namespace hyped {

using hyped::utils::io::SPI;
using utils::Logger;
using utils::io::GPIO;
using data::NavigationVector;

namespace sensors {

class Imu : public ImuInterface {
 public:
  Imu(Logger& log, uint32_t pin, uint8_t acc_scale = 0x08);
  ~Imu();
  /*
   *  @brief Returns if the sensor is online
   *
   *  @return true if the sensor is online
   */
  bool isOnline() override {
    return whoAmI();
  }
  /*
   *  @brief Get the Imu data and update the pointer
   */
  void getData(ImuData* data) override;

  /**
   * @brief calculates number of bytes in FIFO and reads number of full sets (6 bytes) into vector of ImuData
   * See data.hpp for ImuData struct
   *
   * @param data ImuData vector to read number of full sets into
   * @return 0 if empty
   */
  int readFifo(std::vector<ImuData>& data);

  /**
   * @brief Get the Temperature from the IMU
   *
   * @param temp - the variable to be updated
   */
  void getTemperature(int* temp);

 private:
  /*
   *  @brief Sets the range for the accelerometer by writing to the IMU given the write register address
   */
  void setAcclScale(int scale);
  void init();

  /**
   * @brief Resets and enables fifo after sleeping 500 ms, frame size is set to 6 for xyz acceleration
   */
  void enableFifo();

  /**
   * @brief used for SPI chipselect with GPIO pin for IMU
   */
  void select();

  /**
   * @brief used for SPI chipselect with GPIO pin for IMU
   */
  void deSelect();

  /**
   * @brief checks what address the sensor is at
   *
   * @return true
   * @return false
   */
  bool whoAmI();

  /**
   * @brief chipselects and and writes data (byte) to register address
   *
   * @param write_reg write register address
   * @param write_data byte of data to write
   */
  void writeByte(uint8_t write_reg, uint8_t write_data);

  /**
   * @brief uses chip select and reads necessary data
   *
   * @param read_reg read register address
   * @param read_data pointer to data desired to read
   */
  void readByte(uint8_t read_reg, uint8_t *read_data);

  /**
   * @brief same as readByte but with desired length
   *
   * @param read_reg
   * @param read_buff
   * @param length number of bytes to read
   */
  void readBytes(uint8_t read_reg, uint8_t *read_buff, uint8_t length);

 private:
  SPI&    spi_;
  Logger& log_;
  GPIO    gpio_;
  uint32_t pin_;
  uint8_t acc_scale_;
  double  acc_divider_;
  bool    is_online_;
  static const uint64_t time_start;
  size_t kFrameSize_;               // initialised as 6 in enableFifo()
};

}}  // namespace hyped::sensors


#endif  // SENSORS_IMU_HPP_
