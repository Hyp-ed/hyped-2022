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

void RpmRegulator::setCoefficients(std::vector<double> coefficients)
{
  coefficients_ = coefficients;
}

int32_t RpmRegulator::calculateOptimalRpm(const data::nav_t actual_velocity)
{
  double beta1 = coefficients_.at(0);
  double beta0 = coefficients_.at(1);

  // polynomial values from simulation
  return std::round(beta1 * actual_velocity + beta0);
  return 0;
}

int32_t RpmRegulator::step(const int32_t optimal_rpm, const int32_t actual_rpm)
{
  if (actual_rpm < optimal_rpm) { return std::round(optimal_rpm * 0.1); }
  return std::round(optimal_rpm * -0.05);
}

}  // namespace hyped::propulsion
