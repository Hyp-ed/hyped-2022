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
  using KeyenceDataArray = std::array<data::StripeCounter, data::Sensors::kNumKeyence>;

  /**
   * @brief Construct a new Stripe Counter object
   *
   * @param log System logger
   * @param data Central data struct
   * @param displacement_uncertainty Reference to uncertainty in displacement, read only
   * @param velocity_uncertainty Reference to uncertainty in velocity, this is written to
   * @param stripe_distance Distance between two stripes
   */
  explicit StripeHandler(utils::Logger &log, data::Data &data,
                         const data::nav_t &displacement_uncertainty,
                         data::nav_t &velocity_uncertainty, const data::nav_t stripe_distance);

  /**
   * @brief Check if stripe has been detected and changes the displacement
   *        and velocity input from the navigation class accordingly
   *
   * @param displacement Current displacement, reported by other sensors
   * @param velocity Current velocity
   * @param real Whether or not the sensors are real
   */
  void queryKeyence(data::nav_t &displacement, data::nav_t &velocity, const bool real);
  /**
   * @brief Checks if submodule should enter kCriticalFailure
   *
   * @param displ Current displacement, reported by other sensors
   *
   * @return bool to enter kCriticalFailure or not
   */
  bool checkFailure(const data::nav_t displacement);
  /**
   * @brief Sets the initial time and keyence data
   *        Occurs on the first iteration when nav-
   *        main is moved to accelerating state
   *
   * @param init_time initial timestamp
   */
  void setInit(const uint32_t init_time);
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
   * @brief Get the current displacement, measured by the stripe sensor
   * @return Number of stripes encountered * Distance of each stripe
   */
  data::nav_t getStripeDisplacement() const;
  /**
   * @brief Get the offset between given displacement and stripe count displacement
   * @param displacement Current displacement, reported by other sensors
   * @return Difference between strip count displacement and given displacement
   */
  data::nav_t getDisplacementOffset(const data::nav_t displacement) const;

 private:
  // TODO(Sury): Justify these thresholds
  // The divisor that determines the stripe uncertainty, e.g kStripeDist / kStripeUncertainty
  static constexpr data::nav_t kStripeUncertainty = 5.;
  // The multiplier for the max displacement offset e.g kStripeDist * kMaxStripeDifference
  static constexpr int kMaxStripeDifference = 4;

  // Distance between stripes
  const data::nav_t kStripeDist;

  /**
   * @brief Update nav data
   *
   * @param displacement Current displacement, reported by other sensors
   * @param velocity Current velocity
   */
  void updateNavData(data::nav_t &displacement, data::nav_t &velocity);
  /**
   * @brief update prev_readings and current readings_
   */
  void updateReadings();

  // Central logging and data struct
  utils::Logger &log_;
  data::Data &data_;

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
