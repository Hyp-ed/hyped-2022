/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description: Main file for Imu
 *
 *    Copyright 2019 HYPED
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
#include "utils/interface_factory.hpp"

// user bank addresse
constexpr uint8_t kRegBankSel               = 0x7F;

// Accelerometer addresses
constexpr uint8_t kAccelXoutH               = 0x2D;   // userbank 0

constexpr uint8_t kAccelConfig              = 0x14;   // userbank 2
constexpr uint8_t kAccelScale               = 0x04;   // +/- 4g

constexpr uint8_t kWhoAmIImu                = 0x00;   // sensor to be at this address, userbank 0
// data to be at these addresses when read from sensor else not initialised
constexpr uint8_t kWhoAmIResetValue         = 0xEA;   // userbank 0

// Power Management
constexpr uint8_t kPwrMgmt1           = 0x06;   // userbank 0
constexpr uint8_t kPwrMgmt2           = 0x07;   // userbank 0

// Configuration
constexpr uint8_t kReadFlag                 = 0x80;   // unable to find in datasheet

constexpr uint8_t kLpConfig                 = 0x05;   // userbank 0

// Configuration bits Imu
// constexpr uint8_t kBitsFs250Dps             = 0x00;
// constexpr uint8_t kBitsFs500Dps             = 0x08;
// constexpr uint8_t kBitsFs1000Dps            = 0x10;
// constexpr uint8_t kBitsFs2000Dps            = 0x18;
constexpr uint8_t kBitsFs2G                 = 0x00;   // for accel_config
constexpr uint8_t kBitsFs4G                 = 0x02;
constexpr uint8_t kBitsFs8G                 = 0x04;
constexpr uint8_t kBitsFs16G                = 0x06;

// Resets the device to defaults
constexpr uint8_t kBitHReset                = 0x80;    // for pwr_mgmt


// values for FIFO
constexpr uint8_t kFifoReset                = 0x68;   // userbank 0
// constexpr uint8_t kFifoEnable1              = 0x66;   // userbank 0
constexpr uint8_t kFifoEnable2              = 0x67;   // userbank 0
constexpr uint8_t kFifoMode                 = 0x69;
constexpr uint8_t kFifoCountH               = 0x70;   // userbank 0
constexpr uint8_t kFifoRW                   = 0x72;   // userbank 0
constexpr uint8_t kUserCtrl                 = 0x03;   // to reset and enable FIFO
// constexpr uint8_t kIntEnable2 = 0x12;    // userbank 0, for FIFO overflow, read = 0x10


namespace hyped {

utils::io::gpio::Direction kDirection = utils::io::gpio::kOut;
using utils::concurrent::Thread;
using utils::math::OnlineStatistics;
using data::NavigationVector;

namespace sensors {

Imu::Imu(Logger& log, uint32_t pin, bool is_fifo)
    : spi_(SPI::getInstance()),
    log_(log),
    gpio_(pin, kDirection, log),
    pin_(pin),
    is_fifo_(is_fifo),
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

  selectBank(0);

  writeByte(kPwrMgmt1, kBitHReset);   // Reset Device
  Thread::sleep(200);
  // Test connection
  bool check_init = whoAmI();

  writeByte(kPwrMgmt2, 0x07);         // enable acc, disable gyro
  writeByte(kLpConfig, 0x20);         // enable duty cycle acc
  writeByte(kUserCtrl, 0x80);         // enable DMP

  selectBank(2);
  // DLPF
  writeByte(kAccelConfig, 0x09);    // LPF and DLPF configuration
  setAcclScale();

  enableFifo();

  if (check_init) {
    log_.INFO("Imu", "Imu sensor %d created. Initialisation complete.", pin_);
  } else {
    log_.ERR("Imu", "ERROR: Imu sensor %d not initialised.", pin_);
  }
}

void Imu::enableFifo()
{
  selectBank(0);
  writeByte(kFifoReset, 0x0F);
  Thread::sleep(500);
  uint8_t data;
  readByte(kUserCtrl, &data);
  writeByte(kUserCtrl, data | 0x40);       // enable FIFO
  // TODO(anyone): look into SRAM
  writeByte(kFifoMode, 0x01);              // do not write when full
  writeByte(kFifoEnable2, 0x10);
  uint8_t check_enable;
  readByte(kFifoEnable2, &check_enable);   // only for acceleration xyz
  if (check_enable == 0x10) {
    log_.INFO("Imu", "FIFO Enabled");
  } else {
    log_.ERR("Imu", "ERROR: FIFO not enabled");
  }
  kFrameSize_ = 6;
}

bool Imu::whoAmI()
{
  uint8_t data;
  int send_counter;

  for (send_counter = 1; send_counter < 10; send_counter++) {
    readByte(kWhoAmIImu, &data);
    log_.DBG1("Imu", "Imu connected to SPI, data: %d", data);
    if (data == kWhoAmIResetValue) {
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

void Imu::selectBank(uint8_t switch_bank)
{
  writeByte(kRegBankSel, switch_bank << 4);
  user_bank_ = switch_bank;
  log_.DBG1("Imu", "User bank switched to %u", user_bank_);
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

void Imu::setAcclScale()
{
  uint8_t data;
  readByte(kAccelConfig, &data);
  writeByte(kAccelConfig, data | kAccelScale);

  switch (kAccelScale) {
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

int Imu::readFifo(ImuData* data)
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
    for (size_t i = 0; i < (fifo_size/kFrameSize_); i++) {    // make sure is less than array size
      readBytes(kFifoRW, buffer, kFrameSize_);
      axcounts = (((int16_t)buffer[0]) << 8) | buffer[1];     // 2 byte acc data for xyz
      aycounts = (((int16_t)buffer[2]) << 8) | buffer[3];
      azcounts = (((int16_t)buffer[4]) << 8) | buffer[5];

      // convert to floats for accel_data
      value_x = static_cast<float>(axcounts);
      value_y = static_cast<float>(aycounts);
      value_z = static_cast<float>(azcounts);

      // put data in struct and add to data vector (param)
      data->operational = is_online_;
      data->fifo[i][0] = value_x/acc_divider_  * 9.80665;
      data->fifo[i][1] = value_y/acc_divider_  * 9.80665;
      data->fifo[i][2] = value_z/acc_divider_  * 9.80665;
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
    if (is_fifo_) {
      int count = readFifo(data);   // TODO(anyone): does this synax work?
      if (count) {
        log_.DBG2("Imu", "Fifo filled");
      } else {
        log_.DBG2("Imu", "Fifo empty");
      }
    } else {
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
    }
  } else {
    // Try and turn the sensor on again
    log_.ERR("Imu", "Sensor not operational, trying to turn on sensor");
    init();
  }
}

namespace {
ImuInterface* createImu()
{
  Logger log(true, -1);
  return new Imu(log, 66, false);
}

int regImu = utils::InterfaceFactory<ImuInterface>::registerCreator("Imu", createImu);
}   // namespace ::

}}  // namespace hyped::sensors
