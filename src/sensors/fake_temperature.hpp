#pragma once

#include "interface.hpp"

#include <string>

#include <utils/logger.hpp>

namespace hyped {

using data::Data;
using utils::Logger;

namespace sensors {

class FakeTemperature : public TemperatureInterface {
 public:
  /**
   * @brief Construct a new Fake Temperature object
   *
   * @param log
   * @param is_fail
   */
  FakeTemperature(Logger &log, bool is_fail);

  /**
   * @brief returns int representation
   *
   * @return int temperature degrees C
   */
  int getData() override;

  /**
   * @brief waits for acceleration, generate random time for failure
   */
  void run() override;

 private:
  Data &data_;
  utils::Logger &log_;

  /**
   * @brief dependent on is_fail_, set to fail value
   */
  void checkFailure();

  // values degrees C
  int failure_;
  int success_;
  data::TemperatureData temp_;

  bool is_fail_;

  uint64_t acc_start_time_;
  bool acc_started_;
  uint64_t failure_time_;
  bool failure_happened_;
};
}  // namespace sensors
}  // namespace hyped
