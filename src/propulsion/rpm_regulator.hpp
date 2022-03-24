#pragma once

#include <data/data.hpp>

namespace hyped::propulsion {

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
   * @return int32_t - the optimal rpm which the motors should be set to.
   */
  uint32_t calculateRpm(const data::nav_t actual_velocity, const int32_t actual_rpm);

 private:
  /**
   * @brief calculates the optimal rpm based off of the current velocity.
   *
   * @param actual_velocity
   * @return double - optimal rpm
   */
  uint32_t calculateOptimalRpm(const data::nav_t actual_velocity);

  /**
   * @brief calculate the step to increase or decrease the rpm by.
   *
   * @param optimal_rpm - the optimal rpm for our current velocity
   * @param actual_rpm - the rpm of the motor
   * @return int32_t - the step with which to increase the rpm
   */
  uint32_t step(const int32_t optimal_rpm, const int32_t actual_rpm);
};

}  // namespace hyped::propulsion
