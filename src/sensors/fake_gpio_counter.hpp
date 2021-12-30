#pragma once

#include "interface.hpp"

#include <string>
#include <vector>

#include <utils/logger.hpp>

namespace hyped::sensors {

class FakeGpioCounter : public GpioInterface {
 public:
  /**
   * @brief Construct a new Fake Gpio Counter dynamic object
   *
   * @param log
   * @param miss_stripe
   */
  FakeGpioCounter(utils::Logger &log, const bool miss_stripe);

  /**
   * @brief Construct a new Fake Gpio Counter object from file
   *
   * @param log
   * @param miss_stripe
   * @param file_path
   */
  FakeGpioCounter(utils::Logger &log, const bool miss_stripe, const std::string file_path);

  /**
   * @brief returns stripe counter
   *
   * @param stripe_counter data
   */
  void getData(data::StripeCounter &stripe_counter) override;

  bool isOnline() override;

 private:
  /**
   * @brief turns sensor offline if max time reached between stripes by analysing timestamps
   */
  void checkData();
  void readFromFile(const std::string file_path);
  utils::Logger &log_;
  data::Data &data_;

  /**
   * @brief current stripe data
   */
  data::StripeCounter stripe_counter_;

  /**
   * @brief if missed single stripe, set true if does not match navigation data
   */
  bool miss_stripe_;

  /**
   * @brief vector of StripeCounter data read from file
   */
  std::vector<data::StripeCounter> stripe_data_;
  bool is_from_file_;
  uint64_t accel_start_time_;
  bool acc_ref_init_;

  /**
   * @brief used to compare previous stripes if missed stripe
   */
  uint64_t stripe_file_timestamp_;
};

}  // namespace hyped::sensors
