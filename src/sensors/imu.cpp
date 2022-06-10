#include "imu.hpp"

#include <algorithm>
#include <array>
#include <vector>

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/math/statistics.hpp>
#include <utils/system.hpp>

// user bank addresse
static constexpr uint8_t kRegBankSel = 0x7F;

// Accelerometer addresses
static constexpr uint8_t kAccelXoutH = 0x2D;  // userbank 0

static constexpr uint8_t kAccelConfig     = 0x14;  // userbank 2
static constexpr uint8_t kAccelScale      = 0x02;  // +/- 4g
static constexpr uint8_t kAccelSmplrtDiv1 = 0x10;  // userbank 2
static constexpr uint8_t kAccelSmplrtDiv2 = 0x11;  // userbank 2

static constexpr uint8_t kWhoAmIImu = 0x00;  // sensor to be at this address, userbank 0
// data to be at these addresses when read from sensor else not initialised
static constexpr uint8_t kWhoAmIResetValue = 0xEA;  // userbank 0

// Power Management
static constexpr uint8_t kPwrMgmt1 = 0x06;  // userbank 0
static constexpr uint8_t kPwrMgmt2 = 0x07;  // userbank 0

// Configuration
static constexpr uint8_t kReadFlag = 0x80;  // msb is 1 which signifies a read operation for SPI

// Configuration bits Imu
// constexpr uint8_t kBitsFs250Dps             = 0x00;
// constexpr uint8_t kBitsFs500Dps             = 0x08;
// constexpr uint8_t kBitsFs1000Dps            = 0x10;
// constexpr uint8_t kBitsFs2000Dps            = 0x18;
static constexpr uint8_t kBitsFs2G  = 0x00;  // for accel_config
static constexpr uint8_t kBitsFs4G  = 0x02;
static constexpr uint8_t kBitsFs8G  = 0x04;
static constexpr uint8_t kBitsFs16G = 0x06;

// Resets the device to defaults
static constexpr uint8_t kBitHReset = 0x80;  // for pwr_mgmt

// values for FIFO
// constexpr uint8_t kFifoEnable1              = 0x66;   // userbank 0
static constexpr uint8_t kFifoEnable2   = 0x67;  // userbank 0
static constexpr uint8_t kFifoReset     = 0x68;  // userbank 0
static constexpr uint8_t kFifoMode      = 0x69;  // userbank 0
static constexpr uint8_t kFifoCountH    = 0x70;  // userbank 0
static constexpr uint8_t kFifoRW        = 0x72;  // userbank 0
static constexpr uint8_t kDataRdyStatus = 0x74;  // userbank 0
static constexpr uint8_t kUserCtrl      = 0x03;  // userbank 0
// constexpr uint8_t kIntEnable2 = 0x12;    // userbank 0, for FIFO overflow, read = 0x10

namespace hyped::sensors {

static constexpr utils::io::Gpio::Direction kDirection = utils::io::Gpio::Direction::kOut;

Imu::Imu(const uint32_t pin, const bool is_fifo)
    : spi_(utils::io::Spi::getInstance()),
      log_("IMU", utils::System::getSystem().config_.log_level_sensors),
      gpio_(pin, kDirection, log_),
      pin_(pin),
      is_fifo_(is_fifo),
      is_online_(false)
{
  log_.info("started for pin %u", pin);
  init();
}

void Imu::init()
{
  // Set pin high
  gpio_.set();

  selectBank(0);

  writeByte(kPwrMgmt1, kBitHReset);  // Reset Device
  utils::concurrent::Thread::sleep(200);
  // Test connection
  bool check_init = whoAmI();

  writeByte(kPwrMgmt1, 0x01);  // autoselect clock source
  // writeByte(kIntPinConfig, 0xC0); // int pin config

  writeByte(kPwrMgmt2, 0x07);  // enable acc, disable gyro

  // Digital Motion Processor disabled to enable FIFO
  // writeByte(kUserCtrl, 0x08);         // reset DMP
  // writeByte(kUserCtrl, 0x80);         // enable DMP

  // acceleration configurations
  selectBank(2);

  writeByte(kAccelConfig, 0x01);  // reset val

  // DLPF
  // writeByte(kAccelConfig, 0x09);    // LPF and DLPF configuration
  writeByte(kAccelConfig, 0x08);  // reference low pass filter config table

  setAcclScale();

  enableFifo();

  if (check_init) {
    log_.info("sensor %d created. Initialisation complete.", pin_);
    selectBank(0);
  } else {
    log_.error("sensor %d not initialised.", pin_);
  }
}

void Imu::enableFifo()
{
  selectBank(0);
  // reset: assert and de-assert
  writeByte(kFifoReset, 0x1F);
  utils::concurrent::Thread::sleep(200);
  writeByte(kFifoReset, 0x00);
  uint8_t data;
  readByte(kUserCtrl, &data);
  writeByte(kUserCtrl, data | 0x40);  // enable FIFO

  // writeByte(kFifoMode, 0x1F);              // do not write when full
  writeByte(kFifoMode, 0x00);       // override old data, data stream
  writeByte(kFifoEnable2, 0x10);    // acc data to fifo enabled
  writeByte(kDataRdyStatus, 0x0F);  // acc values written to FIFO

  uint8_t check_enable;
  readByte(kUserCtrl, &check_enable);  // in user control

  if (check_enable == (data | 0x40)) {
    log_.info("enabled FIFO for pin %u", pin_);
  } else {
    log_.error("failed to enable FIFO for pin %u", pin_);
  }
}

bool Imu::whoAmI()
{
  uint8_t data;
  int send_counter;

  for (send_counter = 1; send_counter < 10; send_counter++) {
    readByte(kWhoAmIImu, &data);
    log_.debug("connected to SPI, data: %d", data);
    if (data == kWhoAmIResetValue) {
      is_online_ = true;
      break;
    } else {
      log_.debug("Cannot initialise. Who am I is incorrect");
      is_online_ = false;
      utils::concurrent::Thread::yield();
    }
  }

  if (!is_online_) { log_.error("Cannot initialise who am I. Sensor %d offline", pin_); }
  return is_online_;
}

Imu::~Imu()
{
  log_.info("stopped for pin %u", pin_);
}

void Imu::selectBank(uint8_t switch_bank)
{
  writeByte(kRegBankSel, (switch_bank << 4));
  user_bank_ = switch_bank;
  log_.debug("User bank switched to %u", user_bank_);
}

void Imu::writeByte(uint8_t write_reg, uint8_t write_data)
{
  // ',' instead of ';' is to inform the compiler not to reorder function calls
  // chip selects signals must have exact ordering with respect to the spi access
  select(), spi_.write(write_reg, &write_data, 1), deSelect();
}

void Imu::readByte(uint8_t read_reg, uint8_t *read_data)
{
  select(), spi_.read(read_reg | kReadFlag, read_data, 1), deSelect();
}

void Imu::readBytes(uint8_t read_reg, uint8_t *read_data, uint8_t length)
{
  select(), spi_.read(read_reg | kReadFlag, read_data, length), deSelect();
}

void Imu::select()
{
  gpio_.clear();
}
void Imu::deSelect()
{
  gpio_.set();
}

void Imu::setAcclScale()
{
  // userbank 2
  uint8_t data;
  readByte(kAccelConfig, &data);
  writeByte(kAccelConfig, data | kAccelScale);
  // set accel sample rate divider to maximise sample rate (1125 Hz)
  writeByte(kAccelSmplrtDiv1, 0x00);
  writeByte(kAccelSmplrtDiv2, 0x00);

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

int Imu::readFifo(data::ImuData &data)
{
  if (is_online_) {
    data.fifo.clear();
    // get fifo size
    uint8_t buffer[kFrameSize];
    uint8_t size_buffer[2];
    readBytes(kFifoCountH, reinterpret_cast<uint8_t *>(size_buffer), 2);  // from count H/L
    // convert big->little endian of count (2 bytes)
    uint16_t fifo_size = (((uint16_t)(size_buffer[0] & 0x1F)) << 8) | (size_buffer[1]);

    if (fifo_size == 0) {
      log_.debug("FIFO EMPTY");
      return 0;
    }
    log_.debug("Buffer size = %d", fifo_size);
    int16_t axcounts, aycounts, azcounts;  // include negative int
    float value_x, value_y, value_z;
    log_.debug("iterating = %lu", fifo_size / kFrameSize);
    for (size_t i = 0; i < fifo_size / kFrameSize; ++i) {  // make sure is less than array size
      readBytes(kFifoRW, buffer, kFrameSize);
      axcounts = (((int16_t)buffer[0]) << 8) | buffer[1];  // 2 byte acc data for xyz
      aycounts = (((int16_t)buffer[2]) << 8) | buffer[3];
      azcounts = (((int16_t)buffer[4]) << 8) | buffer[5];

      value_x = static_cast<float>(axcounts);
      value_y = static_cast<float>(aycounts);
      value_z = static_cast<float>(azcounts);

      // put data in struct and add to data vector (param)
      data::NavigationVector imu_data;
      data.operational = is_online_;
      imu_data[0]      = value_x / acc_divider_ * 9.80665;
      imu_data[1]      = value_y / acc_divider_ * 9.80665;
      imu_data[2]      = value_z / acc_divider_ * 9.80665;
      data.fifo.push_back(imu_data);
      // log_.INFO("Imu-FIFO", "FIFO readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 0,
      // imu_data[0], imu_data[1], imu_data[2]);   // NOLINT
    }
    return 1;
  } else {
    // Try and turn the sensor on again
    log_.error("Sensor not operational, trying to turn on sensor");
    init();
    return 0;
  }
}

data::ImuData Imu::getData()
{
  data::ImuData imu_data;
  if (is_online_) {
    if (is_fifo_) {
      int count = readFifo(imu_data);
      if (count) {
        log_.debug("Fifo filled");
      } else {
        log_.debug("Fifo empty");
      }
    } else {
      log_.debug("Getting Imu data");
      uint8_t response[6];
      int16_t bit_data;
      float value;
      std::array<float, 3> acceleration_data;

      // Reading six bytes - first two give x-acceleration, next two give y-acceleration, last two give z-acceleration (high and low byte pairs)
      readBytes(kAccelXoutH, response, 6); 
      for (size_t i = 0; i < 3; ++i) {
        bit_data                = ((int16_t)response[i * 2] << 8) | response[i * 2 + 1];
        value                   = static_cast<float>(bit_data);
        acceleration_data.at(i) = value / acc_divider_ * 9.80665;
      }
      imu_data.operational = is_online_;
      imu_data.acc         = acceleration_data;
    }
  } else {
    imu_data.operational = false;
  }
  return imu_data;
}

}  // namespace hyped::sensors
