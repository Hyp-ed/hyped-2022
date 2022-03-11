#include "rpm_regulator.hpp"

#include <cmath>

namespace hyped::propulsion {

RpmRegulator::RpmRegulator()
{
}

static int32_t RpmRegulator::calculateRpm(const data::nav_t actual_velocity,
                                          const int32_t actual_rpm)
{
  const int32_t optimal_rpm = calculateOptimalRpm(actual_velocity);
  if (actual_rpm == optimal_rpm) { return actual_rpm; }
  const int32_t target = actual_rpm + step(optimal_rpm, actual_rpm);
  return std::max(target, 0);
}

static int32_t RpmRegulator::calculateOptimalRpm(const data::nav_t actual_velocity)
{
  // polynomial values from simulation
  return std::round(0.32047 * actual_velocity * actual_velocity + 297.72578 * actual_velocity
                    + 1024.30824);
}

static int32_t RpmRegulator::step(const int32_t optimal_rpm, const int32_t actual_rpm)
{
  if (actual_rpm < optimal_rpm) { return std::round(optimal_rpm * 0.1); }
  return std::round(optimal_rpm * -0.05);
}

}  // namespace hyped::propulsion
