#include "RPM_Regulator.hpp"

#include <vector>

namespace hyped {

namespace motor_control {

RPMRegulator::RPMRegulator(Logger &log) : log_(log), current_index(0), failure(false)
{
}

int32_t RPMRegulator::calculateRPM(const int32_t actual_velocity, const int32_t actual_rpm,
                                   const int32_t actual_current, const int32_t actual_temperature)
{
  const int32_t optimal_rpm = calculateOptimalRPM(actual_velocity);
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

int32_t RPMRegulator::calculateOptimalRPM(const int32_t actual_velocity)
{
  int32_t optimal_rpm = std::round(0.32047 * actual_velocity * actual_velocity
                                   + 297.72578 * actual_velocity + 1024.30824);
  return optimal_rpm;
}

int32_t RPMRegulator::step(const int32_t optimal_rpm, const bool direction)
{
  if (direction) {
    return std::round(optimal_rpm * 0.1);
  } else {
    return std::round(optimal_rpm * 0.05);
  }
}

bool RPMRegulator::getFailure()
{
  return failure;
}
}  // namespace motor_control
}  // namespace hyped
