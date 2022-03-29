#include "rpm_regulator.hpp"

#include <cmath>

#include "utils/math/regression.hpp"

namespace hyped::propulsion {

RpmRegulator::RpmRegulator()
{
}

int32_t RpmRegulator::calculateRpm(const data::nav_t actual_velocity, const int32_t actual_rpm)
{
  const int32_t optimal_rpm = calculateOptimalRpm(actual_velocity);
  if (actual_rpm == optimal_rpm) { return actual_rpm; }
  const int32_t target = actual_rpm + step(optimal_rpm, actual_rpm);
  return std::max(target, 0);
}

int32_t RpmRegulator::calculateOptimalRpm(const data::nav_t actual_velocity)
{
  hyped::utils::math::Regression regression;
  const double beta1 = regression.Coefficients.beta1;
  const double beta0 = regression.Coefficients.beta0;

  // polynomial values from simulation
  return std::round(beta1 * actual_velocity + beta0);
}

int32_t RpmRegulator::step(const int32_t optimal_rpm, const int32_t actual_rpm)
{
  if (actual_rpm < optimal_rpm) { return std::round(optimal_rpm * 0.1); }
  return std::round(optimal_rpm * -0.05);
}

}  // namespace hyped::propulsion
