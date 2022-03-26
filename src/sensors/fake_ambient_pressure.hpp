#pragma once

#include "interface.hpp"

#include <utils/logger.hpp>

namespace hyped::sensors {

class FakeAmbientPressure : public IAmbientPressure {
 public:
  /**
   * @brief Construct a new Fake AmbientPressure object
   *
   * @param is_fail
   */
  FakeAmbientPressure(const bool is_fail);

  /**
   * @brief returns int representation
   *
   * @return int pressure mbar
   */
  uint16_t getData() const override;

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

  // values mbar
  const int failure_;
  const int success_;
  data::AmbientPressureData pressure_data_;

  bool is_fail_;

  uint64_t acc_start_time_;
  bool acc_started_;
  uint64_t failure_time_;
  bool failure_happened_;
};
}  // namespace hyped::sensors
