#pragma once

#include <cmath>

#include <array>
#include <cstdint>

#include <data/data.hpp>
#include <data/data_point.hpp>
#include <utils/logger.hpp>
#include <utils/math/statistics.hpp>
#include <utils/timer.hpp>

namespace hyped {
namespace navigation {

class StripeHandler {
 public:
  using KeyenceDataArray = std::array<data::StripeCounter, Sensors::kNumKeyence>;

  /**
   * @brief Construct a new Stripe Counter object
   *
   * @param log System logger
   * @param data Central data struct
   * @param displacement_uncertainty Reference to uncertainty in displacement, read only
   * @param velocity_uncertainty Reference to uncertainty in velocity, this is written to
   * @param stripe_distance Distance between two stripes
   */
  explicit StripeHandler(utils::Logger &log, data::Data &data, const data::nav_t &displacement_uncertainty, data::nav_t &velocity_uncertainty,
                         const data::nav_t stripe_distance);

  /**
   * @brief Check if stripe has been detected and changes the displacement
   *        and velocity input from the navigation class accordingly
   *
   * @param displacement Current displacement
   * @param velocity Current velocity
   * @param real Whether or not the sensors are real
   */
  void queryKeyence(data::nav_t &displacement, data::nav_t &velocity, const bool real);
  /**
   * @brief Checks if submodule should enter kCriticalFailure
   *
   * @param displ Current displacement from all sensors for comparison
   *
   * @return bool to enter kCriticalFailure or not
   */
  bool checkFailure(data::nav_t displacement);
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
  uint32_t getStripeCount() const;
  /**
   * @brief Get the current number of failures
   *
   * @return number of failures
   */
  uint32_t getFailureCount() const;
  /**
   * @brief Get the offset between given displacement and stripe count displacement
   * @param displacement To find offset between
   * @return Difference between strip count displacement and given displacement
   */
  data::nav_t getDisplacementOffset(data::nav_t &displacement) const;

 private:
  // Distance between stripes
  const data::nav_t kStripeDist;

  /**
   * @brief Update nav data
   *
   * @param displacement Current displacement
   * @param velocity Current velocity
   */
  void updateNavData(data::nav_t &displacement, data::nav_t &velocity);
  /**
   * @brief update prev_readings
   */
  void updatePrevReadings();
  /**
   * @brief update current readings
   */
  void updateNewReadings();

  // Central logging and data struct
  Logger &log_;
  Data &data_;

  // Number of stripes hit & most recent timestamp
  data::DataPoint<uint32_t> stripe_counter_;
  // Keyence data read
  KeyenceDataArray readings_;
  // Previous keyence data for comparison
  KeyenceDataArray prev_readings_;
  // Number of significant sensor disagreements
  uint32_t num_missed_stripes_;

  // displacement uncertainty, const because this is never written to
  const data::nav_t &displacement_uncertainty_;
  // velocity uncertainty
  data::nav_t &velocity_uncertainty_;
  // initial timestamp
  uint32_t init_time_;
};
}  // namespace navigation
}  // namespace hyped

