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

#include <algorithm>
#include "navigation/stripe_handler.hpp"

namespace hyped {
namespace navigation {

StripeHandler::StripeHandler(Logger& log, Data& data, const NavigationType& displ_unc,
                             NavigationType& vel_unc, const NavigationType stripe_dist)
    : kStripeDist(stripe_dist),
      log_(log),
      data_(data),
      stripe_counter_(0, 0),
      n_missed_stripes_(0),
      displ_unc_(displ_unc),
      vel_unc_(vel_unc)
{}

void StripeHandler::getReadings()
{
  readings_ = data_.getSensorsKeyenceData();
}

void StripeHandler::updateReadings()
{
  prev_readings_ = readings_;
}

void StripeHandler::set_init(uint32_t init_time)
{
  init_time_ = init_time;
  prev_readings_ = data_.getSensorsKeyenceData();
}

uint16_t StripeHandler::getStripeCount()
{
  return stripe_counter_.value;
}

uint8_t StripeHandler::getFailureCount()
{
  return n_missed_stripes_;
}

bool StripeHandler::checkFailure(NavigationType displ)
{
  // Failure if more than one disagreement
  if (n_missed_stripes_ > 1) {
    log_.ERR("NAV", "More than one large IMU/Keyence disagreement, entering kCriticalFailure");
    return true;
  }
  if (displ - stripe_counter_.value*kStripeDist > 4 * kStripeDist) {
    log_.ERR("NAV", "IMU distance at least 3 * kStripeDist ahead, entering kCriticalFailure.");
    return true;
  }
  return false;
}

void StripeHandler::updateNavData(NavigationType& displ, NavigationType& vel)
{
  NavigationType displ_offset = displ - stripe_counter_.value*kStripeDist;
  vel -= displ_offset*1e6/(stripe_counter_.timestamp - init_time_);
  displ -= displ_offset;
}

void StripeHandler::queryKeyence(NavigationType& displ, NavigationType& vel, bool real)
{
  getReadings();

  for (int i = 0; i < Sensors::kNumKeyence; i++) {
    // Check new readings and ensure new stripe has been hit
    if (prev_readings_[i].count.value == readings_[i].count.value ||
         readings_[i].count.timestamp - stripe_counter_.timestamp < 1e5)
         continue;
    stripe_counter_.value++;
    stripe_counter_.timestamp = readings_[i].count.timestamp;
    if (!real) stripe_counter_.timestamp = utils::Timer::getTimeMicros();

    NavigationType minimum_uncertainty = kStripeDist / 5.;
    NavigationType allowed_uncertainty = std::max(displ_unc_, minimum_uncertainty);
    NavigationType displ_offset = displ - stripe_counter_.value*kStripeDist;

    // Allow up to one missed stripe
    if (displ_offset > kStripeDist - allowed_uncertainty &&
        displ_offset < kStripeDist + allowed_uncertainty &&
        displ > stripe_counter_.value*kStripeDist + 0.5*kStripeDist) {
      stripe_counter_.value++;
      displ_offset -= kStripeDist;
    }
    // Too large disagreement
    if (std::abs(displ_offset) > 2 * allowed_uncertainty) {
      log_.INFO("NAV", "Displ_change: %.3f, allowed uncertainty: %.3f", displ_offset,
        allowed_uncertainty);
      n_missed_stripes_++;
      n_missed_stripes_ += floor(abs(displ_offset) / kStripeDist);
    }
    // Lower the uncertainty in velocity
    vel_unc_ -= abs(displ_offset*1e6/(stripe_counter_.timestamp - init_time_));
    log_.DBG("NAV", "Stripe detected!");
    log_.DBG1("NAV", "Timestamp difference: %d", stripe_counter_.timestamp - init_time_);
    log_.DBG1("NAV", "Timestamp currently:  %d", stripe_counter_.timestamp);

    // Ensure velocity uncertainty is positive
    vel_unc_ = abs(vel_unc_);
    updateNavData(displ, vel);

    break;
  }
  // Update old keyence readings with current ones
  updateReadings();
}
}}  // namespace hyped::navigation
