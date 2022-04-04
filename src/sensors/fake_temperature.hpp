#pragma once

#include "temperature.hpp"

#include <string>

#include <utils/logger.hpp>

namespace hyped::sensors {

class FakeTemperature : public ITemperature {
 public:
  /**
   * @brief Construct a new Fake Temperature object
   * @param is_fail
   */
  FakeTemperature(const bool is_fail);

  /**
   * @brief returns int representation
   *
   * @return int temperature degrees C
   */
  uint8_t getData() const override;

  /**
   * @brief waits for acceleration, generate random time for failure
   */
  void run() override;

 private:
  data::Data &data_;
  utils::Logger log_;

  /**
   * @brief dependent on is_fail_, set to fail value
   */
  void checkFailure();

  // values degrees C
  int failure_;
  int success_;
  data::TemperatureData temperature_data_;

  const bool is_fail_;

  uint64_t acc_start_time_;
  bool acc_started_;
  uint64_t failure_time_;
  bool failure_happened_;
};
}  // namespace hyped::sensors
