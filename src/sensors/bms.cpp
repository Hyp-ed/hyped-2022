/*
 * Authors: M. Kristien and Gregory Dayao
 * Organisation: HYPED
 * Date: 12. April 2018
 * Description:
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

#include "sensors/bms.hpp"

#include "utils/logger.hpp"
#include "utils/timer.hpp"

namespace hyped {

namespace sensors {

std::vector<uint8_t> BMS::existing_ids_;    // NOLINT [build/include_what_you_use]
int16_t BMS::current_ = 0;
BMS::BMS(uint8_t id, Logger& log)
    : Thread(log),
      data_({}),
      id_(id),
      id_base_(bms::kIdBase + (bms::kIdIncrement * id_)),
      last_update_time_(0),
      can_(Can::getInstance()),
      running_(false)
{
  ASSERT(id < data::Batteries::kNumLPBatteries);
  // verify this BMS unit has not been instantiated
  for (uint8_t i : existing_ids_) {
    if (id == i) {
      log_.ERR("BMS", "BMS %d already exists, duplicate unit instantiation", id);
      return;
    }
  }
  existing_ids_.push_back(id);

  // tell CAN about yourself
  can_.registerProcessor(this);
  can_.start();

  running_ = true;
}

BMS::~BMS()
{
  running_ = false;
  join();
}

void BMS::request()
{
  // send request CanFrame
  utils::io::can::Frame message;
  message.id        = bms::kIdBase + (bms::kIdIncrement * id_);
  message.extended  = true;
  message.len       = 2;
  message.data[0]   = 0;
  message.data[1]   = 0;

  int sent = can_.send(message);
  if (sent) {
    log_.DBG1("BMS", "module %u: request message sent", id_);
  } else {
    log_.ERR("BMS", "module %u error: request message not sent", id_);
  }
}

void BMS::run()
{
  log_.INFO("BMS", "module %u: starting BMS", id_);
  while (running_) {
    request();
    sleep(bms::kPeriod);
  }
  log_.INFO("BMS", "module %u: stopped BMS", id_);
}

bool BMS::hasId(uint32_t id, bool extended)
{
  if (!extended) return false;  // this BMS only understands extended IDs

  // LP BMS CAN messages
  if (id_base_ <= id && id < id_base_ + bms::kIdSize) return true;

  // LP current CAN message
  if (id == 0x28) return true;

  return false;
}

void BMS::processNewData(utils::io::can::Frame& message)
{
  log_.DBG1("BMS", "module %u: received CAN message with id %d", id_, message.id);

  // check current CAN message
  if (message.id == 0x28) {
    if (message.len < 3) {
      log_.ERR("BMS", "module %u: current reading not enough data", id_);
      return;
    }

    current_ = (message.data[1] << 8) | (message.data[2]);
    return;
  }

  log_.DBG2("BMS", "message data[0,1] %d %d", message.data[0], message.data[1]);
  uint8_t offset = message.id - (bms::kIdBase + (bms::kIdIncrement * id_));
  switch (offset) {
    case 0x1:   // cells 1-4
      for (int i = 0; i < 4; i++) {
        data_.voltage[i] = (message.data[2*i] << 8) | message.data[2*i + 1];
      }
      break;
    case 0x2:   // cells 5-7
      for (int i = 0; i < 3; i++) {
        data_.voltage[4 + i] = (message.data[2*i] << 8) | message.data[2*i + 1];
      }
      break;
    case 0x3:   // ignore, no cells connected
      break;
    case 0x4:   // temperature
      data_.temperature = message.data[0] - bms::Data::kTemperatureOffset;
      break;
    default:
      log_.ERR("BMS", "received invalid message, id %d, CANID %d, offset %d",
          id_, message.id, offset);
  }

  last_update_time_ = utils::Timer::getTimeMicros();
}

bool BMS::isOnline()
{
  // consider online if the data has been updated in the last second
  return (utils::Timer::getTimeMicros() - last_update_time_) < 1000000;
}

void BMS::getData(BatteryData* battery)
{
  battery->voltage = 0;
  for (uint16_t v: data_.voltage) battery->voltage += v;
  battery->voltage    /= 100;  // scale to dV from mV
  battery->average_temperature = data_.temperature;
  if (battery->average_temperature == -40) battery->average_temperature = 0;    // if temp offline
  battery->current     = current_ - 0x800000;  // offset provided by datasheet
  battery->current    /= 100;   // scale to dA from mA

  // not used, initialised to zero
  battery->low_temperature = 0;
  battery->high_temperature = 0;
  battery->low_voltage_cell = 0;
  battery->high_voltage_cell = 0;

  // charge calculation
  if (battery->voltage >= 252) {                                     // constant high
    battery->charge = 95;
  } else if (252 > battery->voltage && battery->voltage >= 210) {    // linear high
    battery->charge = static_cast<uint8_t>(std::round((battery->voltage - 198.8) * (25/14)));
  } else if (210 > battery->voltage && battery->voltage >= 207) {    // binomial low
    battery->charge = 15;
  } else if (207 > battery->voltage && battery->voltage >= 200) {    // binomial low
    battery->charge = 10;
  } else if (200 > battery->voltage && battery->voltage >= 189) {    // binomial low
    battery->charge = 5;
  } else {                                                           // constant low
    battery->charge = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// BMSHP
////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<uint16_t> BMSHP::existing_ids_;   // NOLINT [build/include_what_you_use]

BMSHP::BMSHP(uint16_t id, Logger& log)
    : log_(log),
      can_id_(id*2 + bms::kHPBase),
      thermistor_id_(id + bms::kThermistorBase),
      cell_id_(id + bms::kCellBase),
      local_data_ {},
      last_update_time_(0)
{
  // verify this BMSHP unit has not been instantiated
  for (uint16_t i : existing_ids_) {
    if (id == i) {
      log_.ERR("BMSHP", "BMSHP %d already exists, duplicate unit instantiation", id);
      return;
    }
  }
  existing_ids_.push_back(id);

  // tell CAN about yourself
  Can::getInstance().registerProcessor(this);
  Can::getInstance().start();
}

bool BMSHP::isOnline()
{
  // consider online if the data has been updated in the last second
  return (utils::Timer::getTimeMicros() - last_update_time_) < 1000000;
}

void BMSHP::getData(BatteryData* battery)
{
  *battery = local_data_;
}

bool BMSHP::hasId(uint32_t id, bool extended)
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

void BMSHP::processNewData(utils::io::can::Frame& message)
{
  // thermistor expansion module
  if (message.id == thermistor_id_) {   // C
    local_data_.low_temperature     = message.data[1];
    local_data_.high_temperature    = message.data[2];
    local_data_.average_temperature = message.data[3];
  }

  log_.DBG2("BMSHP", "High Temp: %d, Average Temp: %d, Low Temp: %d",
    local_data_.high_temperature,
    local_data_.average_temperature,
    local_data_.low_temperature);

  // voltage, current, charge, and isolation 1:1 configured
  // low_voltage_cell and high_voltage_cell 10:1 configured
  // message format is expected to look like this:
  // [ voltageH , volageL , currentH , currentL , charge , lowVoltageCellH , lowVoltageCellL ]
  // [ highVoltageCellH , highVoltageCellL , IsolationADCH , Isolation ADCL]
  if (message.id == can_id_) {
    local_data_.voltage     = (message.data[0] << 8) | message.data[1];           // dV
    local_data_.current     = (message.data[2] << 8) | message.data[3];           // dV
    local_data_.charge      = (message.data[4]) * 0.5;                            // %
    local_data_.low_voltage_cell  = ((message.data[5] << 8) | message.data[6]);   // mV
  } else if (message.id == static_cast<uint16_t>(can_id_ + 1)) {
    local_data_.high_voltage_cell = ((message.data[0] << 8) | message.data[1]);   // mV
    uint16_t imd_reading = ((message.data[2] << 8) | message.data[3]);            // mV
    log_.DBG2("BMSHP", "Isolation ADC: %u", imd_reading);
    if (imd_reading > 4000) {      // 4 volts for safe isolation
      local_data_.imd_fault = true;
    } else {
      local_data_.imd_fault = false;
    }
  }
  last_update_time_ = utils::Timer::getTimeMicros();

  // individual cell voltages, configured at 100ms refresh rate
  if (message.id == cell_id_) {
    int cell_num = static_cast<int>(message.data[0]);   // get any value
    local_data_.cell_voltage[cell_num] = (message.data[1] << 8) | message.data[2];
    local_data_.cell_voltage[cell_num] /=10;            // mV
  }

  log_.DBG2("BMSHP", "Cell voltage: %u", local_data_.cell_voltage[0]);
  log_.DBG2("BMSHP", "received data Volt,Curr,Char,low_v,high_v: %u,%u,%u,%u,%u",
    local_data_.voltage,
    local_data_.current,
    local_data_.charge,
    local_data_.low_voltage_cell,
    local_data_.high_voltage_cell);
}
}}  // namespace hyped::sensors
