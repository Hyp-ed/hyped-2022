#include "rpm_regulator.hpp"

#include <vector>

namespace hyped {

namespace motor_control {

RpmRegulator::RpmRegulator() : current_index(0), failure(false)
{
}

int32_t RpmRegulator::calculateRpm(const data::nav_t actual_velocity, const int32_t actual_rpm,
                                   const int32_t actual_current, const int32_t actual_temperature)
{
  const int32_t optimal_rpm = calculateOptimalRpm(actual_velocity);
  if (actual_temperature <= MAX_TEMPERATURE) {
    if (actual_current < MAX_CURRENT && actual_temperature < MAX_TEMPERATURE
        && actual_rpm < optimal_rpm) {
      return actual_rpm + step(optimal_rpm, true);
    } else if (actual_current > MAX_CURRENT || actual_temperature > MAX_TEMPERATURE
               || actual_rpm > optimal_rpm) {
      int32_t target = actual_rpm - step(optimal_rpm, false);
      if (target < 0) {
        return 0;
      } else {
        return target;
      }
    } else {
      return actual_rpm;
    }
  } else {
    return actual_rpm;
  }
}

int32_t RpmRegulator::calculateOptimalRpm(const int32_t actual_velocity)
{
  int32_t optimal_rpm = std::round(0.32047 * actual_velocity * actual_velocity
                                   + 297.72578 * actual_velocity + 1024.30824);
  return optimal_rpm;
}

int32_t RpmRegulator::step(const int32_t optimal_rpm, const bool direction)
{
  if (direction) {
    return std::round(optimal_rpm * 0.1);
  } else {
    return std::round(optimal_rpm * 0.05);
  }
}

bool RpmRegulator::getFailure()
{
  return failure;
}
}  // namespace motor_control
}  // namespace hyped
