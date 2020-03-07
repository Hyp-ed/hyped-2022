/*
 * Author:Justus Rudolph
 * Organisation: HYPED
 * Date: 07/03/2020
 * Description: Stripe counter class for navigation.
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "navigation/stripe_count.hpp"

namespace hyped {
namespace navigation {

StripeCount::StripeCount(Logger& log, Data& data, NavigationType& displ_unc,
  NavigationType& vel_unc, NavigationType stripe_dist)
  : log_(log),
    data_(data),
    stripe_counter_(0, 0),
    failure_counter_(0),
    stripe_dist_(stripe_dist),
    displ_unc_(displ_unc),
    vel_unc_(vel_unc)
    {}

void StripeCount::getReadings()
{
  readings_ = data_.getSensorsKeyenceData();
}

void StripeCount::updateReadings()
{
  prev_readings_ = readings_;
}

void StripeCount::set_init(uint32_t init_time)
{
  init_time_ = init_time;
  prev_readings_ = data_.getSensorsKeyenceData();
}

uint16_t StripeCount::getStripeCount()
{
  return stripe_counter_.value;
}

uint8_t StripeCount::getFailureCount()
{
  return failure_counter_;
}

bool StripeCount::checkFailure(NavigationType displ)
{
  // Failure if more than one disagreement
  if (failure_counter_ > 1) {
    log_.ERR("NAV", "More than one large IMU/Keyence disagreement, entering kCriticalFailure");
    return true;
  }
  if (displ - stripe_counter_.value*stripe_dist_ > 4 * stripe_dist_) {
    log_.ERR("NAV", "IMU distance at least 3 * stripe_dist_ ahead, entering kCriticalFailure.");
    return true;
  }
  return false;
}

void StripeCount::updateNavData(NavigationType& displ, NavigationType& vel)
{
  NavigationType displ_change = displ - stripe_counter_.value*stripe_dist_;
  vel -= displ_change*1e6/(stripe_counter_.timestamp - init_time_);
  displ -= displ_change;
}

void StripeCount::queryKeyence(NavigationType& displ, NavigationType& vel, bool real)
{
  getReadings();

  for (int i = 0; i < Sensors::kNumKeyence; i++) {
    // Check new readings
    if (prev_readings_[i].count.value != readings_[i].count.value &&
         readings_[i].count.timestamp - stripe_counter_.timestamp > 1e5) {
      stripe_counter_.value++;
      stripe_counter_.timestamp = readings_[i].count.timestamp;
      if (!real) stripe_counter_.timestamp = utils::Timer::getTimeMicros();

      NavigationType allowed_uncertainty = displ_unc_;
      NavigationType minimum_uncertainty = stripe_dist_ / 5.;

      if (displ_unc_ < minimum_uncertainty) allowed_uncertainty = minimum_uncertainty;
      NavigationType displ_change = displ - stripe_counter_.value*stripe_dist_;

      // Allow up to one missed stripe
      if (displ_change > stripe_dist_ - allowed_uncertainty &&
          displ_change < stripe_dist_ + allowed_uncertainty &&
          displ > stripe_counter_.value*stripe_dist_ + 0.5*stripe_dist_) {
        stripe_counter_.value++;
        displ_change -= stripe_dist_;
      }
      // Too large disagreement
      if ((displ_change < (-2) * allowed_uncertainty) ||
          (displ_change > 2 * allowed_uncertainty))
      {
        log_.INFO("NAV", "Displ_change: %.3f, allowed uncertainty: %.3f", displ_change,
          allowed_uncertainty);
        failure_counter_++;
        failure_counter_ += floor(abs(displ_change) / stripe_dist_);
      }
      // Lower the uncertainty in velocity
      vel_unc_ -= abs(displ_change*1e6/(stripe_counter_.timestamp - init_time_));
      log_.DBG("NAV", "Stripe detected!");
      log_.DBG1("NAV", "Timestamp difference: %d", stripe_counter_.timestamp - init_time_);
      log_.DBG1("NAV", "Timestamp currently:  %d", stripe_counter_.timestamp);

      // Ensure velocity uncertainty is positive
      vel_unc_ = abs(vel_unc_);
      updateNavData(displ, vel);

      break;
    }
  }
  // Update old keyence readings with current ones
  updateReadings();
}
}}

