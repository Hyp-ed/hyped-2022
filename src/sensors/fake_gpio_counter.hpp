/*
 * Author: Gregory Dayao and Jack Horsburgh
 * Organisation: HYPED
 * Date: 1/4/19
 * Description:
 *
 *    Copyright 2019 HYPED
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

#ifndef SENSORS_FAKE_GPIO_COUNTER_HPP_
#define SENSORS_FAKE_GPIO_COUNTER_HPP_

#include <string>
#include <utils/logger.hpp>
#include <vector>

#include "interface.hpp"

namespace hyped {

using data::Data;
using data::StripeCounter;
using utils::Logger;

namespace sensors {

class FakeGpioCounter : public GpioInterface {
 public:
  /**
   * @brief Construct a new Fake Gpio Counter dynamic object
   *
   * @param log
   * @param miss_stripe
   */
  FakeGpioCounter(utils::Logger &log, bool miss_stripe);

  /**
   * @brief Construct a new Fake Gpio Counter object from file
   *
   * @param log
   * @param miss_stripe
   * @param file_path
   */
  FakeGpioCounter(utils::Logger &log, bool miss_stripe, std::string file_path);

  /**
   * @brief returns stripe counter
   *
   * @param stripe_counter data
   */
  void getData(StripeCounter *stripe_counter) override;

  bool isOnline() override;

 private:
  /**
   * @brief turns sensor offline if max time reached between stripes by analysing timestamps
   */
  void checkData();
  void readFromFile(std::vector<StripeCounter> &data);
  Logger &log_;
  Data &data_;

  /**
   * @brief current stripe data
   */
  StripeCounter stripe_count_;

  /**
   * @brief if missed single stripe, set true if does not match navigation data
   */
  bool miss_stripe_;

  std::string file_path_;

  /**
   * @brief vector of StripeCounter data read from file
   */
  std::vector<StripeCounter> stripe_data_;
  bool is_from_file_;
  uint64_t accel_start_time_;
  bool acc_ref_init_;

  /**
   * @brief used to compare previous stripes if missed stripe
   */
  uint64_t stripe_file_timestamp_;
};

}  // namespace sensors
}  // namespace hyped

#endif  // SENSORS_FAKE_GPIO_COUNTER_HPP_
