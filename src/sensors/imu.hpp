#pragma once

#include "sensor.hpp"

#include <vector>

#include <data/data.hpp>
#include <utils/io/gpio.hpp>
#include <utils/io/spi.hpp>
#include <utils/logger.hpp>

namespace hyped::sensors {

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

class Imu : public IImu {
 public:
  Imu(const uint32_t pin, const bool is_fifo);
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
  data::ImuData getData() override;

  /**
   * @brief calculates number of bytes in FIFO and reads number of full sets (6 bytes) into vector
   * of ImuData See data.hpp for ImuData struct
   *
   * @param data ImuData vector to read number of full sets into
   * @return 0 if empty
   */
  int readFifo(data::ImuData &data);

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

  void selectBank(const uint8_t switch_bank);

  /**
   * @brief chipselects and and writes data (byte) to register address
   *
   * @param write_reg write register address
   * @param write_data byte of data to write
   */
  void writeByte(const uint8_t write_reg, const uint8_t write_data);

  /**
   * @brief uses chip select and reads necessary data
   *
   * @param read_reg read register address
   * @param read_data pointer to data desired to read
   */
  void readByte(const uint8_t read_reg, uint8_t *read_data);

  /**
   * @brief same as readByte but with desired length
   *
   * @param read_reg
   * @param read_buff
   * @param length number of bytes to read
   */
  void readBytes(const uint8_t read_reg, uint8_t *read_buff, const uint8_t length);

 private:
  utils::io::Spi &spi_;
  utils::Logger log_;
  utils::io::Gpio gpio_;
  uint32_t pin_;
  bool is_fifo_;
  double acc_divider_;
  bool is_online_;
  uint8_t user_bank_;
  static const uint64_t time_start;
  static constexpr size_t kFrameSize = 6;
};

}  // namespace hyped::sensors
