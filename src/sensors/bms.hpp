/*
 * Authors: M. Kristien and Gregory Dayao
 * Organisation: HYPED
 * Date: 12. April 2018
 * Description:
 * Battery Management System abstraction. Each object corresponds to one low-powered BMS unit.
 * Each unit is identified by a unique ID, which is also used to infer IDs of CAN messages.
 *
 * Configuration and local data structure are encapsulated in namespace bms.
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

#ifndef SENSORS_BMS_HPP_
#define SENSORS_BMS_HPP_

#include <cstdint>
#include <vector>

#include "sensors/interface.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/io/can.hpp"
#include "utils/system.hpp"
#include "utils/utils.hpp"

namespace hyped {

// Forward declarations
namespace utils { class Logger; }
namespace utils { namespace io { class Can; } }
namespace utils { namespace io { class CanProccesor; } }
namespace utils { namespace io { namespace can { struct Frame; } } }

namespace sensors {

using utils::Logger;
using utils::io::Can;
using utils::io::CanProccesor;
using utils::concurrent::Thread;
using data::Data;

namespace bms {
// how often shall request messages be sent
constexpr uint32_t kFreq    = 4;           // in Hz
constexpr uint32_t kPeriod  = 1000/kFreq;  // in milliseconds

// what is the CAN ID space for BMS units
constexpr uint16_t kIdBase      = 300;     // Base for ids of CAN messages related to BMS
constexpr uint16_t kIdIncrement = 10;      // increment of base dependent on id_
constexpr uint16_t kIdSize      = 5;       // size of id-space of BMS-CAN messages
/**
 * Bases of IDs of CAN messagese for a BMS unit are calculated as follows:
 * base = kIdBase + (kIdIncrement * id_)
 */

/**
 * LP: Notches:    0        1        2
 *     ID:      301-304, 311-314, 321-324
 *     Hex:     12D-130, 137-13A, 141-144
 * HP: ID:      1712-13, 1714-15
 *     Hex:     6B0-6B1, 6B2-6B3
 * Therm ID:   406451072  406451073
 *       Hex:  0x1839F380 0x1839F381
 */

constexpr uint16_t kHPBase      = 0x6B0;    // CAN id for high power BMSHP
constexpr uint64_t kThermistorBase = 0x1839F380;  // HP Thermistor expansion module
constexpr uint16_t kCellBase = 0x36;        // BMS Broadcast ID

struct Data {
  static constexpr uint8_t kTemperatureOffset = 40;
  static constexpr uint8_t kCellNum           = 7;
  uint16_t voltage[kCellNum];
  int8_t   temperature;
};

}   // namespace bms

class BMS : public Thread, public CanProccesor, public BMSInterface {
  friend Can;

 public:
  /**
   * @brief Construct a new BMS object
   * @param id  - should correspond to the id sessing on the actual BMS unit
   * @param log - for printing nice messages
   */
  BMS(uint8_t id, Logger& log = utils::System::getLogger());

  ~BMS();

  /**
   * @brief Implement virtual run() from Thread
   * This is used to periodically send request CAN messages
   */
  void run() override;

  // From BMSInterface
  bool isOnline() override;
  void getData(BatteryData* battery) override;

  // From CanProcessor interface
  bool hasId(uint32_t id, bool extended) override;

 private:
  /**
   * @brief Send request CAN message to update data periodically
   */
  void request();

  /**
   * @brief To be called by CAN receive side. BMS processes received CAN
   * message and updates its local data
   *
   * @param message received CAN message to be processed
   */
  void processNewData(utils::io::can::Frame& message) override;

 private:
  bms::Data       data_;
  uint8_t         id_;                // my BMS id in (0,..,15)
  uint32_t        id_base_;           // my starting CAN id
  uint64_t        last_update_time_;  // stores arrival time of CAN response

  // for request thread
  Can&            can_;
  bool            running_;

  // for making sure only one object per BMS unit exist
  static std::vector<uint8_t> existing_ids_;
  static int16_t current_;
  NO_COPY_ASSIGN(BMS);
};

class BMSHP : public CanProccesor, public BMSInterface {
  friend Can;

 public:
  /**
   * @brief Construct a new BMSHP object
   * @param id  - should directly correspond to the CAN id to be used
   * @param log - for printing nice messages
   */
  BMSHP(uint16_t id, Logger& log = utils::System::getLogger());

  // from BMSInterface
  bool isOnline() override;
  void getData(BatteryData* battery) override;

  // from CanProcessor
  bool hasId(uint32_t id, bool extended) override;

 private:
  void processNewData(utils::io::can::Frame& message) override;

 private:
  Logger&         log_;
  uint16_t        can_id_;            // CAN id to be used
  uint64_t        thermistor_id_;     // thermistor expansion module CAN id
  uint16_t        cell_id_;           // broadcast message ID
  BatteryData     local_data_;        // stores values from CAN
  uint64_t        last_update_time_;  // stores arrival time of CAN message
  // for making sure only one object per BMS unit exist
  static std::vector<uint16_t> existing_ids_;
  NO_COPY_ASSIGN(BMSHP);
};

}}  // namespace hyped::sensors

#endif  // SENSORS_BMS_HPP_
