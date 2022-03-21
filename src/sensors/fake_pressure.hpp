#pragma once

#include "interface.hpp"

#include <string>

#include <utils/logger.hpp>

namespace hyped::sensors {

class FakePressure : public IPressure {
 public:
  /**
   * @brief Construct a new Fake Pressure object
   *
   * @param log
   * @param is_fail
   */
  FakePressure(utils::Logger &log, bool is_fail);

  /**
   * @brief returns int representation
   *
   * @return int pressure mbar
   */
  uint8_t getData() override;

  /**
   * @brief waits for acceleration, generate random time for failure
   */
  void run() override;

 private:
  data::Data &data_;
  utils::Logger &log_;

  /**
   * @brief dependent on is_fail_, set to fail value
   */
  void checkFailure();

  // values mbar
  int failure_;
  int success_;
  data::PressureData pressure_;

  bool is_fail_;

  uint64_t acc_start_time_;
  bool acc_started_;
  uint64_t failure_time_;
  bool failure_happened_;
};
}  // namespace hyped::sensors
