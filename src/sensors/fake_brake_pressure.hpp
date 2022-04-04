#pragma once

#include "brake_pressure.hpp"

#include <string>

#include <utils/logger.hpp>

namespace hyped::sensors {

class FakeBrakePressure : public IBrakePressure {
 public:
  /**
   * @brief Construct a new Fake Brake Pressure object
   *
   * @param log
   * @param is_fail
   */
  FakeBrakePressure(const bool is_fail);

  /**
   * @brief returns int representation
   *
   * @return int pressure mBars
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

  // values bars
  int failure_;
  int success_;
  data::BrakePressureData pressure_data_;

  bool is_fail_;

  uint64_t acc_start_time_;
  bool acc_started_;
  uint64_t failure_time_;
  bool failure_happened_;
};
}  // namespace hyped::sensors
