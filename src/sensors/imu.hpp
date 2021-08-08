#pragma once

#include <utils/io/gpio.hpp>
#include <utils/io/spi.hpp>
#include <utils/logger.hpp>
#include <vector>

#include "interface.hpp"

namespace hyped {

using data::NavigationVector;
using hyped::utils::io::SPI;
using utils::Logger;
using utils::io::GPIO;

namespace sensors {

class Imu : public ImuInterface {
 public:
  Imu(Logger &log, uint32_t pin, bool is_fifo);
  ~Imu();
  /*
   *  @brief Returns if the sensor is online
   *
   *  @return true if the sensor is online
   */
  bool isOnline() override { return whoAmI(); }
  /*
   *  @brief Get the Imu data and update the pointer
   */
  void getData(ImuData *data) override;

  /**
   * @brief calculates number of bytes in FIFO and reads number of full sets (6 bytes) into vector
   * of ImuData See data.hpp for ImuData struct
   *
   * @param data ImuData vector to read number of full sets into
   * @return 0 if empty
   */
  int readFifo(ImuData *data);

 private:
  /*
   *  @brief Sets the range for the accelerometer by writing to the IMU given the write register
   * address
   */
  void setAcclScale();
  void init();

  /**
   * @brief Resets and enables fifo after sleeping 500 ms, frame size is set to 6 for xyz
   * acceleration
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

  void selectBank(uint8_t switch_bank);

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
  SPI &spi_;
  Logger &log_;
  GPIO gpio_;
  uint32_t pin_;
  bool is_fifo_;
  double acc_divider_;
  bool is_online_;
  uint8_t user_bank_;
  static const uint64_t time_start;
  size_t kFrameSize_;  // initialised as 6 in enableFifo()
};

}  // namespace sensors
}  // namespace hyped
