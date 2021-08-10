#pragma once

#define MAX_RPM 6000
#define MAX_TEMP 150
#define MAX_CURRENT 1500  // mA

#include "controller.hpp"

#include <cstdlib>
#include <vector>

#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped {

using std::vector;

using motor_control::Controller;
using utils::Logger;

namespace motor_control {

class RPM_Regulator {
 public:
  /*
   * @brief Construct a new rpm regulator object
   * @param log
   */
  explicit RPM_Regulator(Logger &log);
  /**
   * @brief Calculate the optimal rpm based on criteria from all the motors
   *        as well optimal values produced by simulations.
   *
   * @param act_velocity - the actual velocity of the pod from navigation
   * @param act_rpm - average rpm of all the motors
   * @param act_current - max current (mA) drawn out of all the motors
   * @param act_temp - max temperature out of all the motors
   * @return int32_t - the optimal rpm which the motors should be set to.
   */
  int32_t calculateRPM(int32_t act_velocity, int32_t act_rpm, int32_t act_current,
                       int32_t act_temp);

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
   * @param act_velocity
   * @return double - optimal rpm
   */
  int32_t calculateOptimalRPM(int32_t act_velocity);

  /**
   * @brief calculate the step to increase or decrease the rpm by.
   *
   * @param opt_rpm - the optimal rpm for our current velocity
   * @param direction - the direction that the step must go in: true for pos false for neg.
   * @return int32_t - the step with which to increase the rpm
   */
  int32_t step(int32_t opt_rpm, bool direction);

  Logger &log_;
  int32_t current_index;
  bool failure;
};

}  // namespace motor_control
}  // namespace hyped
