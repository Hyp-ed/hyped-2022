#pragma once

#include "sensor.hpp"

#include <cstdint>
#include <vector>

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/io/can.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>
#include <utils/utils.hpp>

namespace hyped::sensors {

class IBms : public ISensor {
 public:
  /**
   * @brief empty virtual deconstructor for proper deletion of derived classes
   */
  virtual ~IBms() {}

  /**
   * @brief Get Battery data
   * @param battery - output pointer to be filled by this sensor
   */
  virtual data::BatteryData getData() = 0;
};

namespace bms {
// how often shall request messages be sent
constexpr uint32_t kFreq   = 4;             // in Hz
constexpr uint32_t kPeriod = 1000 / kFreq;  // in milliseconds

// what is the CAN ID space for BMS units
constexpr uint16_t kIdBase      = 300;  // Base for ids of CAN messages related to BMS
constexpr uint16_t kIdIncrement = 10;   // increment of base dependent on id_
constexpr uint16_t kIdSize      = 5;    // size of id-space of BMS-CAN messages
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

constexpr uint16_t kHPBase         = 0x6B0;       // CAN id for high power HighPowerBms
constexpr uint64_t kThermistorBase = 0x1839F380;  // HP Thermistor expansion module
constexpr uint16_t kCellBase       = 0x36;        // BMS Broadcast ID

struct Data {
  static constexpr uint8_t kTemperatureOffset = 40;
  static constexpr uint8_t kCellNum           = 7;
  uint16_t voltage[kCellNum];
  int8_t temperature;
};

}  // namespace bms

class Bms : public utils::concurrent::Thread, public utils::io::CanProcessor, public IBms {
  friend utils::io::Can;

 public:
  /**
   * @brief Construct a new BMS object
   * @param id  - should correspond to the id sessing on the actual BMS unit
   * @param log - for printing nice messages
   */
  Bms(uint8_t id, utils::Logger &log = utils::System::getLogger());

  ~Bms();

  /**
   * @brief Implement virtual run() from Thread
   * This is used to periodically send request CAN messages
   */
  void run() override;

  // From IBms
  bool isOnline() override;
  data::BatteryData getData() override;

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
  void processNewData(utils::io::can::Frame &message) override;

 private:
  bms::Data data_;
  uint8_t id_;                 // my BMS id in (0,..,15)
  uint32_t id_base_;           // my starting CAN id
  uint64_t last_update_time_;  // stores arrival time of CAN response

  // for request thread
  utils::io::Can &can_;
  bool running_;

  // for making sure only one object per BMS unit exist
  static std::vector<uint8_t> existing_ids_;
  static int16_t current_;
  NO_COPY_ASSIGN(Bms)
};

class HighPowerBms : public utils::io::CanProcessor, public IBms {
  friend utils::io::Can;

 public:
  /**
   * @brief Construct a new HighPowerBms object
   * @param id  - should directly correspond to the CAN id to be used
   * @param log - for printing nice messages
   */
  HighPowerBms(uint16_t id, utils::Logger &log = utils::System::getLogger());

  // from IBms
  bool isOnline() override;
  data::BatteryData getData() override;

  // from CanProcessor
  bool hasId(uint32_t id, bool extended) override;

 private:
  void processNewData(utils::io::can::Frame &message) override;

 private:
  utils::Logger &log_;
  uint16_t can_id_;                 // CAN id to be used
  uint64_t thermistor_id_;          // thermistor expansion module CAN id
  uint16_t cell_id_;                // broadcast message ID
  data::BatteryData battery_data_;  // stores values from CAN
  uint64_t last_update_time_;       // stores arrival time of CAN message
  // for making sure only one object per BMS unit exist
  static std::vector<uint16_t> existing_ids_;
  NO_COPY_ASSIGN(HighPowerBms)
};

}  // namespace hyped::sensors
