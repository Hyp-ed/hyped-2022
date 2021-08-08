#include "RPM_regulator.hpp"

#include <vector>

namespace hyped {

namespace motor_control {

RPM_Regulator::RPM_Regulator(Logger &log) : log_(log), current_index(0), failure(false)
{
}

int32_t RPM_Regulator::calculateRPM(int32_t act_velocity, int32_t act_rpm, int32_t act_current,
                                    int32_t act_temp)
{
  int32_t opt_rpm = calculateOptimalRPM(act_velocity);
  if (act_temp <= MAX_TEMP) {
    if (act_current < MAX_CURRENT && act_temp < MAX_TEMP && act_rpm < opt_rpm) {
      return act_rpm + step(opt_rpm, true);
    } else if (act_current > MAX_CURRENT || act_temp > MAX_TEMP || act_rpm > opt_rpm) {
      int32_t target = act_rpm - step(opt_rpm, false);
      if (target < 0) {
        return 0;
      } else {
        return target;
      }
    } else {
      return act_rpm;
    }
  } else {
    return act_rpm;
  }
}

int32_t RPM_Regulator::calculateOptimalRPM(int32_t act_velocity)
{
  int32_t opt_rpm
    = std::round(0.32047 * act_velocity * act_velocity + 297.72578 * act_velocity + 1024.30824);
  return opt_rpm;
}

int32_t RPM_Regulator::step(int32_t opt_rpm, bool direction)
{
  if (direction) {
    return std::round(opt_rpm * 0.1);
  } else {
    return std::round(opt_rpm * 0.05);
  }
}

bool RPM_Regulator::getFailure()
{
  return failure;
}
}  // namespace motor_control
}  // namespace hyped
