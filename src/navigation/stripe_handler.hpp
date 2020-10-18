/*
 * Author:Justus Rudolph
 * Organisation: HYPED
 * Date: 07/03/2020
 * Description: Stripe counter class for navigation.
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef NAVIGATION_STRIPE_HANDLER_HPP_
#define NAVIGATION_STRIPE_HANDLER_HPP_

#include <cstdint>
#include <array>
#include <cmath>

#include "data/data.hpp"
#include "data/data_point.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"
#include "utils/math/statistics.hpp"

namespace hyped {

using data::Data;
using data::DataPoint;
using data::ModuleStatus;
using data::NavigationType;
using data::NavigationVector;
using data::Motors;
using data::Sensors;
using utils::Logger;
using std::array;

namespace navigation {

class StripeHandler {
 public:
  typedef array<data::StripeCounter, Sensors::kNumKeyence> KeyenceDataArray;

  /**
   * @brief Construct a new Stripe Counter object
   *
   * @param log System logger
   * @param data Central data struct
   * @param displ_unc Reference to uncertainty in displacement, read only
   * @param vel_unc Reference to uncertainty in velocity, this is written to
   * @param stripe_dist Distance between two stripes
   */
  explicit StripeHandler(Logger& log, Data& data, const NavigationType& displ_unc,
                         NavigationType& vel_unc, const NavigationType stripe_dist);

  /**
   * @brief Check if stripe has been detected and changes the displacement
   *        and velocity input from the navigation class accordingly
   *
   * @param displ Current displacement
   * @param vel Current velocity
   * @param real Whether or not the sensors are real
   */
  void queryKeyence(NavigationType& displ, NavigationType& vel, bool real);
  /**
   * @brief Checks if submodule should enter kCriticalFailure
   *
   * @param displ Current displacement from all sensors for comparison
   *
   * @return bool to enter kCriticalFailure or not
   */
  bool checkFailure(NavigationType displ);
  /**
   * @brief Sets the initial time and keyence data
   *        Occurs on the first iteration when nav-
   *        main is moved to accelerating state
   *
   * @param init_time initial timestamp
   */
  void set_init(uint32_t init_time);
  /**
   * @brief Get the current stripe count
   *
   * @return number of stripes hit
   */
  uint16_t getStripeCount();
  /**
   * @brief Get the current number of failures
   *
   * @return number of failures
   */
  uint8_t getFailureCount();

 private:
  // Distance between stripes
  const NavigationType kStripeDist;

  /**
   * @brief Update nav data
   *
   * @param displ Current displacement
   * @param vel Current velocity
   */
  void updateNavData(NavigationType& displ, NavigationType& vel);
  /**
   * @brief update prev_readings
   */
  void updateReadings();
  /**
   * @brief get current readings
   */
  void getReadings();

  // Central logging and data struct
  Logger& log_;
  Data& data_;

  // Number of stripes hit & most recent timestamp
  DataPoint<uint16_t> stripe_counter_;
  // Keyence data read
  KeyenceDataArray readings_;
  // Previous keyence data for comparison
  KeyenceDataArray prev_readings_;
  // Number of significant sensor disagreements
  uint8_t n_missed_stripes_;

  // displacement uncertainty, const because this is never written to
  const NavigationType& displ_unc_;
  // velocity uncertainty
  NavigationType& vel_unc_;
  // initial timestamp
  uint32_t init_time_;
};
}}
#endif  // NAVIGATION_STRIPE_HANDLER_HPP_
