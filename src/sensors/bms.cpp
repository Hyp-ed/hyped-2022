#include "bms.hpp"

#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

std::vector<uint8_t> Bms::existing_ids_;  // NOLINT [build/include_what_you_use]
int16_t Bms::current_ = 0;
Bms::Bms(uint8_t id, utils::Logger &log)
    : Thread(log),
      data_({}),
      id_(id),
      id_base_(bms::kIdBase + (bms::kIdIncrement * id_)),
      last_update_time_(0),
      can_(utils::io::Can::getInstance()),
      running_(false)
{
  ASSERT(id < data::FullBatteryData::kNumLPBatteries);
  // verify this Bms unit has not been instantiated
  for (uint8_t i : existing_ids_) {
    if (id == i) {
      log_.error("BMS %d already exists, duplicate unit instantiation", id);
      return;
    }
  }
  existing_ids_.push_back(id);

  // tell CAN about yourself
  can_.registerProcessor(this);
  can_.start();

  running_ = true;
}

Bms::~Bms()
{
  running_ = false;
  join();
}

void Bms::request()
{
  // send request CanFrame
  utils::io::can::Frame message;
  message.id       = bms::kIdBase + (bms::kIdIncrement * id_);
  message.extended = true;
  message.len      = 2;
  message.data[0]  = 0;
  message.data[1]  = 0;

  int sent = can_.send(message);
  if (sent) {
    log_.debug("module %u: request message sent", id_);
  } else {
    log_.error("module %u error: request message not sent", id_);
  }
}

void Bms::run()
{
  log_.info("module %u: starting BMS", id_);
  while (running_) {
    request();
    sleep(bms::kPeriod);
  }
  log_.info("module %u: stopped BMS", id_);
}

bool Bms::hasId(uint32_t id, bool extended)
{
  if (!extended) return false;  // this Bms only understands extended IDs

  // LP Bms CAN messages
  if (id_base_ <= id && id < id_base_ + bms::kIdSize) return true;

  // LP current CAN message
  if (id == 0x28) return true;

  return false;
}

void Bms::processNewData(utils::io::can::Frame &message)
{
  log_.debug("module %u: received CAN message with id %d", id_, message.id);

  // check current CAN message
  if (message.id == 0x28) {
    if (message.len < 3) {
      log_.error("module %u: current reading not enough data", id_);
      return;
    }

    current_ = (message.data[1] << 8) | (message.data[2]);
    return;
  }

  log_.debug("message data[0,1] %d %d", message.data[0], message.data[1]);
  uint8_t offset = message.id - (bms::kIdBase + (bms::kIdIncrement * id_));
  switch (offset) {
    case 0x1:  // cells 1-4
      for (size_t i = 0; i < 4; ++i) {
        data_.voltage[i] = (message.data[2 * i] << 8) | message.data[2 * i + 1];
      }
      break;
    case 0x2:  // cells 5-7
      for (size_t i = 0; i < 3; ++i) {
        data_.voltage[4 + i] = (message.data[2 * i] << 8) | message.data[2 * i + 1];
      }
      break;
    case 0x3:  // ignore, no cells connected
      break;
    case 0x4:  // temperature
      data_.temperature = message.data[0] - bms::Data::kTemperatureOffset;
      break;
    default:
      log_.error("received invalid message, id %d, CANID %d, offset %d", id_, message.id, offset);
  }

  last_update_time_ = utils::Timer::getTimeMicros();
}

bool Bms::isOnline()
{
  // consider online if the data has been updated in the last second
  return (utils::Timer::getTimeMicros() - last_update_time_) < 1000000;
}

data::BatteryData Bms::getData()
{
  data::BatteryData battery_data;
  battery_data.voltage = 0;
  for (uint16_t v : data_.voltage)
    battery_data.voltage += v;
  battery_data.voltage /= 100;  // scale to dV from mV
  battery_data.average_temperature = data_.temperature;
  // if temp offline
  if (battery_data.average_temperature == -40) { battery_data.average_temperature = 0; }
  battery_data.current = current_ - 0x800000;  // offset provided by datasheet
  battery_data.current /= 100;                 // scale to dA from mA

  // not used, initialised to zero
  battery_data.low_temperature   = 0;
  battery_data.high_temperature  = 0;
  battery_data.low_voltage_cell  = 0;
  battery_data.high_voltage_cell = 0;

  // charge calculation
  if (battery_data.voltage >= 252) {  // constant high
    battery_data.charge = 95;
  } else if (252 > battery_data.voltage && battery_data.voltage >= 210) {  // linear high
    battery_data.charge
      = static_cast<uint8_t>(std::round((battery_data.voltage - 198.8) * (25 / 14)));
  } else if (210 > battery_data.voltage && battery_data.voltage >= 207) {  // binomial low
    battery_data.charge = 15;
  } else if (207 > battery_data.voltage && battery_data.voltage >= 200) {  // binomial low
    battery_data.charge = 10;
  } else if (200 > battery_data.voltage && battery_data.voltage >= 189) {  // binomial low
    battery_data.charge = 5;
  } else {  // constant low
    battery_data.charge = 0;
  }
  return battery_data;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// HighPowerBms
////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<uint16_t> HighPowerBms::existing_ids_;

HighPowerBms::HighPowerBms(uint16_t id, utils::Logger &log)
    : log_(log),
      can_id_(id * 2 + bms::kHPBase),
      thermistor_id_(id + bms::kThermistorBase),
      cell_id_(id + bms::kCellBase),
      battery_data_{},
      last_update_time_(0)
{
  // verify this HighPowerBms unit has not been instantiated
  for (uint16_t i : existing_ids_) {
    if (id == i) {
      log_.error("HighPowerBms %d already exists, duplicate unit instantiation", id);
      return;
    }
  }
  existing_ids_.push_back(id);

  // tell CAN about yourself
  utils::io::Can::getInstance().registerProcessor(this);
  utils::io::Can::getInstance().start();
}

bool HighPowerBms::isOnline()
{
  // consider online if the data has been updated in the last second
  return (utils::Timer::getTimeMicros() - last_update_time_) < 1000000;
}

data::BatteryData HighPowerBms::getData()
{
  return battery_data_;
}

bool HighPowerBms::hasId(uint32_t id, bool)
{
  // HPBMS
  if (id == can_id_ || id == static_cast<uint16_t>(can_id_ + 1)) return true;

  // CAN ID for broadcast message
  if (id == cell_id_) return true;

  // OBDII ECU ID
  if (id == 0x7E4) return true;
  // unused messages, fault message?
  if (id == 0x6D0 || id == 0x7EC) return true;
  if (id == 0x70 || id == 0x80) return true;

  // Thermistor expansion module
  if (id == thermistor_id_) return true;

  // Thermistor node IDs
  if (id == 0x6B4 || id == 0x6B5) return true;
  // ignore misc thermistor module messages
  if (id == 0x1838F380 || id == 0x18EEFF80) return true;
  if (id == 0x1838F381 || id == 0x18EEFF81) return true;

  return false;
}

void HighPowerBms::processNewData(utils::io::can::Frame &message)
{
  // thermistor expansion module
  if (message.id == thermistor_id_) {  // C
    battery_data_.low_temperature     = message.data[1];
    battery_data_.high_temperature    = message.data[2];
    battery_data_.average_temperature = message.data[3];
  }

  log_.debug("High Temp: %d, Average Temp: %d, Low Temp: %d", battery_data_.high_temperature,
             battery_data_.average_temperature, battery_data_.low_temperature);

  // voltage, current, charge, and isolation 1:1 configured
  // low_voltage_cell and high_voltage_cell 10:1 configured
  // message format is expected to look like this:
  // [ voltageH , volageL , currentH , currentL , charge , lowVoltageCellH , lowVoltageCellL ]
  // [ highVoltageCellH , highVoltageCellL , IsolationADCH , Isolation ADCL]
  if (message.id == can_id_) {
    battery_data_.voltage          = (message.data[0] << 8) | message.data[1];    // dV
    battery_data_.current          = (message.data[2] << 8) | message.data[3];    // dV
    battery_data_.charge           = (message.data[4]) * 0.5;                     // %
    battery_data_.low_voltage_cell = ((message.data[5] << 8) | message.data[6]);  // mV
  } else if (message.id == static_cast<uint16_t>(can_id_ + 1)) {
    battery_data_.high_voltage_cell = ((message.data[0] << 8) | message.data[1]);  // mV
    uint16_t imd_reading            = ((message.data[2] << 8) | message.data[3]);  // mV
    log_.debug("Isolation ADC: %u", imd_reading);
    if (imd_reading > 4000) {  // 4 volts for safe isolation
      battery_data_.imd_fault = true;
    } else {
      battery_data_.imd_fault = false;
    }
  }
  last_update_time_ = utils::Timer::getTimeMicros();

  // individual cell voltages, configured at 100ms refresh rate
  if (message.id == cell_id_) {
    int cell_num                         = static_cast<int>(message.data[0]);  // get any value
    battery_data_.cell_voltage[cell_num] = (message.data[1] << 8) | message.data[2];
    battery_data_.cell_voltage[cell_num] /= 10;  // mV
  }

  log_.debug("Cell voltage: %u", battery_data_.cell_voltage[0]);
  log_.debug("received data Volt,Curr,Char,low_v,high_v: %u,%u,%u,%u,%u", battery_data_.voltage,
             battery_data_.current, battery_data_.charge, battery_data_.low_voltage_cell,
             battery_data_.high_voltage_cell);
}
}  // namespace hyped::sensors
