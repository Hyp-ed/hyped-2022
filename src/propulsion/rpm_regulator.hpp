#pragma once

#include "controller.hpp"

#include <cstdlib>
#include <vector>

#include <utils/logger.hpp>
#include <utils/system.hpp>

static constexpr int kMaximumTemperature = 150;
static constexpr int kMaximumCurrent     = 1500;  // mA

namespace hyped {

namespace motor_control {

class RpmRegulator {
 public:
  /*
   * @brief Construct a new rpm regulator object
   */
  explicit RpmRegulator();
  /**
   * @brief Calculate the optimal rpm based on criteria from all the motors
   *        as well optimal values produced by simulations.
   *
   * @param actual_velocity - the actual velocity of the pod from navigation
   * @param actual_rpm - average rpm of all the motors
   * @param actual_current - max current (mA) drawn out of all the motors
   * @param actual_temperature - max temperature out of all the motors
   * @return int32_t - the optimal rpm which the motors should be set to.
   */
  int32_t calculateRpm(const data::nav_t actual_velocity, const int32_t actual_rpm,
                       const int32_t actual_current, const int32_t actual_temperature);

  /**
   * @brief Get the Failure boolean
   *
   * @return true - if failure has occured
   * @return false - if no failure has occured
   */
  bool isFaulty();

 private:
  /**
   * @brief calculates the optimal rpm based off of the current velocity.
   *
   * @param actual_velocity
   * @return double - optimal rpm
   */
  int32_t calculateOptimalRpm(const int32_t actual_velocity);

  /**
   * @brief calculate the step to increase or decrease the rpm by.
   *
   * @param optimal_rpm - the optimal rpm for our current velocity
   * @param direction - the direction that the step must go in: true for pos false for neg.
   * @return int32_t - the step with which to increase the rpm
   */
  int32_t step(const int32_t optimal_rpm, const bool direction);

  int32_t current_index_;
  bool failure_;
};

}  // namespace motor_control
}  // namespace hyped
