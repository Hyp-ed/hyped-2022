#include "rpm_regulator.hpp"

#include <vector>

namespace hyped {

namespace motor_control {

RpmRegulator::RpmRegulator() : current_index_(0), failure_(false)
{
}

int32_t RpmRegulator::calculateRpm(const data::nav_t actual_velocity, const int32_t actual_rpm,
                                   const int32_t actual_current, const int32_t actual_temperature)
{
  const int32_t optimal_rpm = calculateOptimalRpm(actual_velocity);
  if (actual_current < kMaximumCurrent && actual_rpm == optimal_rpm
      || actual_temperature > kMaximumTemperature) {
    return actual_rpm;
  }
  const int32_t target = actual_rpm + step(optimal_rpm, actual_rpm);
  if (target < 0) { return 0; }
  return target;
}

int32_t RpmRegulator::calculateOptimalRpm(const data::nav_t actual_velocity)
{
  return std::round(0.32047 * actual_velocity * actual_velocity + 297.72578 * actual_velocity
                    + 1024.30824);
}

int32_t RpmRegulator::step(const int32_t optimal_rpm, const int32_t actual_rpm)
{
  if (actual_rpm < optimal_rpm) { return std::round(optimal_rpm * 0.1); }
  return std::round(optimal_rpm * -0.05);
}

bool RpmRegulator::isFaulty()
{
  return failure_;
}
}  // namespace motor_control
}  // namespace hyped
