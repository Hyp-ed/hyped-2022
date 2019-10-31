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
#include <algorithm>
#include <vector>

#include "sensors/imu.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/math/statistics.hpp"


// Accelerometer addresses
constexpr uint8_t kAccelXoutH               = 0x3B;

constexpr uint8_t kAccelConfig              = 0x1C;
constexpr uint8_t kAccelConfig2             = 0x1D;

// Temperature address
constexpr uint8_t kTempOutH                 = 65;

constexpr uint8_t kWhoAmIImu                = 0x75;   // sensor to be at this address
// data to be at these addresses when read from sensor else not initialised
constexpr uint8_t kWhoAmIResetValue1        = 0x71;
constexpr uint8_t kWhoAmIResetValue2        = 0x70;

// Power Management
constexpr uint8_t kMpuRegPwrMgmt1           = 0x6B;

// Configuration
constexpr uint8_t kMpuRegConfig             = 0x1A;

constexpr uint8_t kReadFlag                 = 0x80;

// Configuration bits Imu
// constexpr uint8_t kBitsFs250Dps             = 0x00;
// constexpr uint8_t kBitsFs500Dps             = 0x08;
// constexpr uint8_t kBitsFs1000Dps            = 0x10;
// constexpr uint8_t kBitsFs2000Dps            = 0x18;
constexpr uint8_t kBitsFs2G                 = 0x00;
constexpr uint8_t kBitsFs4G                 = 0x08;
constexpr uint8_t kBitsFs8G                 = 0x10;
constexpr uint8_t kBitsFs16G                = 0x18;

// Resets the device to defaults
constexpr uint8_t kBitHReset                = 0x80;


// values for FIFO
constexpr uint8_t kFifoEnable = 0x23;   // set FIFO enable flags
constexpr uint8_t kFifoCountH = 0x72;   // 2 bytes for H and L registers
constexpr uint8_t kFifoRW = 0x74;
constexpr uint8_t kUserCtrl = 0x6A;     // to reset and enable FIFO
// constexpr uint8_t kIntEnable = 0x38;    // for FIFO overflow, read 0x10 at this register
constexpr uint8_t kFifoAccel = 0x08;


namespace hyped {

utils::io::gpio::Direction kDirection = utils::io::gpio::kOut;
using utils::concurrent::Thread;
using utils::math::OnlineStatistics;
using data::NavigationVector;

namespace sensors {

Imu::Imu(Logger& log, uint32_t pin, uint8_t acc_scale)
    : spi_(SPI::getInstance()),
    log_(log),
    gpio_(pin, kDirection, log),
    pin_(pin),
    acc_scale_(acc_scale),
    is_online_(false)
{
  log_.DBG1("Imu pin: ", "%d", pin);
  log_.INFO("Imu", "Creating Imu sensor now:");
  init();
}

void Imu::init()
{
  // Set pin high
  gpio_.set();

  writeByte(kMpuRegPwrMgmt1, kBitHReset);   // Reset Device
  Thread::sleep(200);
  // Test connection
  bool check_init = whoAmI();

  writeByte(kMpuRegConfig, 0x01);
  writeByte(kAccelConfig2, 0x01);
  setAcclScale(acc_scale_);
  enableFifo();

  if (check_init) {
    log_.INFO("Imu", "FIFO Enabled");
    log_.INFO("Imu", "Imu sensor %d created. Initialisation complete.", pin_);
  } else {
    log_.ERR("Imu", "ERROR: Imu sensor %d not initialised.", pin_);
  }
}

void Imu::enableFifo()
{
  writeByte(kUserCtrl, 0x04);       // Put serial interface to SPI only, FIFO reset
  Thread::sleep(500);
  writeByte(kUserCtrl, 0x40);       // FIFO enable
  writeByte(kFifoEnable, kFifoAccel);
  uint8_t check_enable = 0;
  readByte(kFifoEnable, &check_enable);
  kFrameSize_ = 6;                   // only for acceleration xyz
}

bool Imu::whoAmI()
{
  uint8_t data;
  int send_counter;

  for (send_counter = 1; send_counter < 10; send_counter++) {
    // Who am I checks what address the sensor is at
    readByte(kWhoAmIImu, &data);
    log_.DBG1("Imu", "Imu connected to SPI, data: %d", data);
    if (data == kWhoAmIResetValue1 || data == kWhoAmIResetValue2) {
      is_online_ = true;
      break;
    } else {
      log_.DBG1("Imu", "Cannot initialise. Who am I is incorrect");
      is_online_ = false;
      Thread::yield();
    }
  }

  if (!is_online_) {
    log_.ERR("Imu", "Cannot initialise who am I. Sensor %d offline", pin_);
  }
  return is_online_;
}

Imu::~Imu()
{
  log_.INFO("Imu", "Deconstructing sensor %d object", pin_);
}

void Imu::writeByte(uint8_t write_reg, uint8_t write_data)
{
  // ',' instead of ';' is to inform the compiler not to reorder function calls
  // chip selects signals must have exact ordering with respect to the spi access
  select(),
  spi_.write(write_reg, &write_data, 1),
  deSelect();
}

void Imu::readByte(uint8_t read_reg, uint8_t *read_data)
{
  select(),
  spi_.read(read_reg | kReadFlag, read_data, 1),
  deSelect();
}

void Imu::readBytes(uint8_t read_reg, uint8_t *read_data, uint8_t length)
{
  select(),
  spi_.read(read_reg | kReadFlag, read_data, length),
  deSelect();
}

void Imu::select()
{
  gpio_.clear();
}
void  Imu::deSelect()
{
  gpio_.set();
}

void Imu::setAcclScale(int scale)
{
  writeByte(kAccelConfig, scale);

  switch (scale) {
    case kBitsFs2G:
      acc_divider_ = 16384;
    break;
    case kBitsFs4G:
      acc_divider_ = 8192;
    break;
    case kBitsFs8G:
      acc_divider_ = 4096;
    break;
    case kBitsFs16G:
      acc_divider_ = 2048;
    break;
  }
}

int Imu::readFifo(std::vector<ImuData>& data)
{
  if (is_online_) {
    // get fifo size
    uint8_t buffer[kFrameSize_];
    readBytes(kFifoCountH, reinterpret_cast<uint8_t*>(buffer), 2);    // from count H/L registers
    // convert big->little endian of count (2 bytes)
    size_t fifo_size = (((uint16_t) (buffer[0]&0x0F)) << 8) + (((uint16_t) buffer[1]));

    if (fifo_size == 0) {
      log_.DBG3("Imu-FIFO", "FIFO EMPTY");
      return 0;
    }
    log_.DBG3("Imu-FIFO", "Buffer size = %d", fifo_size);
    int16_t axcounts, aycounts, azcounts;           // include negative int
    float value_x, value_y, value_z;
    for (size_t i = 0; i < (fifo_size/kFrameSize_); i++) {
      readBytes(kFifoRW, buffer, kFrameSize_);
      axcounts = (((int16_t)buffer[0]) << 8) | buffer[1];     // 2 byte acc data for xyz
      aycounts = (((int16_t)buffer[2]) << 8) | buffer[3];
      azcounts = (((int16_t)buffer[4]) << 8) | buffer[5];

      // convert to floats for accel_data
      value_x = static_cast<float>(axcounts);
      value_y = static_cast<float>(aycounts);
      value_z = static_cast<float>(azcounts);

      // put data in struct and add to data vector (param)
      ImuData imu_data;
      imu_data.operational = is_online_;
      imu_data.acc[0] = value_x/acc_divider_  * 9.80665;
      imu_data.acc[1] = value_y/acc_divider_  * 9.80665;
      imu_data.acc[2] = value_z/acc_divider_  * 9.80665;
      data.push_back(imu_data);
    }
    return 1;
  } else {
    // Try and turn the sensor on again
    log_.ERR("Imu-FIFO", "Sensor not operational, trying to turn on sensor");
    init();
    return 0;
  }
}

void Imu::getData(ImuData* data)
{
  if (is_online_) {
    log_.DBG2("Imu", "Getting Imu data");
    auto& acc = data->acc;
    uint8_t response[8];
    int16_t bit_data;
    float value;
    int i;
    float accel_data[3];

    readBytes(kAccelXoutH, response, 8);
    for (i = 0; i < 3; i++) {
      bit_data = ((int16_t) response[i*2] << 8) | response[i*2+1];
      value = static_cast<float>(bit_data);
      accel_data[i] = value/acc_divider_  * 9.80665;
    }
    data->operational = is_online_;
    acc[0] = accel_data[0];
    acc[1] = accel_data[1];
    acc[2] = accel_data[2];
  } else {
    // Try and turn the sensor on again
    log_.ERR("Imu", "Sensor not operational, trying to turn on sensor");
    init();
  }
}

void Imu::getTemperature(int* data)
{
  uint8_t response[2];
  readBytes(kTempOutH, response, 2);

  uint16_t temp = ((response[0] << 8) | response[1])/333.87 + 21;

  *data = static_cast<int>(temp);
}

}}   // namespace hyped::sensors
