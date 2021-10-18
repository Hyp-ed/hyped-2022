#pragma once

#define MAX_RPM 6000
#define MAX_TEMPERATURE 150
#define MAX_CURRENT 1500  // mA

#include "controller.hpp"

#include <cstdlib>
#include <vector>

#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped {

std::vector;

motor_control::Controller;
utils::Logger;

namespace motor_control {

class RPMRegulator {
 public:
  /*
   * @brief Construct a new rpm regulator object
   * @param log
   */
  explicit RPMRegulator(Logger &log);
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
  int32_t calculateRPM(int32_t actual_velocity, int32_t actual_rpm, int32_t actual_current,
                       int32_t actual_temperature);

  /**
   * @brief Get the Failure boolean
   *
   * @return true - if failure has occured
   * @return false - if no failure has occured
   */
  bool getFailure();

 private:
  /**
   * @brief calculates the optimal rpm based off of the current velocity.
   *
   * @param actual_velocity
   * @return double - optimal rpm
   */
  int32_t calculateOptimalRPM(int32_t actual_velocity);

  /**
   * @brief calculate the step to increase or decrease the rpm by.
   *
   * @param optimal_rpm - the optimal rpm for our current velocity
   * @param direction - the direction that the step must go in: true for pos false for neg.
   * @return int32_t - the step with which to increase the rpm
   */
  int32_t step(int32_t optimal_rpm, bool direction);

  Logger &log_;
  int32_t current_index;
  bool failure;
};

}  // namespace motor_control
}  // namespace hyped
