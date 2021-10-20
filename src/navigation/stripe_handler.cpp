#include "stripe_handler.hpp"

#include <algorithm>

namespace hyped {
namespace navigation {

StripeHandler::StripeHandler(utils::Logger &log, data::Data &data,
                             const data::nav_t &displacement_uncertainty,
                             data::nav_t &velocity_uncertainty, const data::nav_t stripe_distance)
    : kStripeDist(stripe_distance),
      log_(log),
      data_(data),
      stripe_counter_(0, 0),
      num_missed_stripes_(0),
      displacement_uncertainty_(displacement_uncertainty),
      velocity_uncertainty_(velocity_uncertainty)
{
}

void StripeHandler::updateReadings()
{
  prev_readings_ = readings_;
  readings_      = data_.getSensorsKeyenceData();
}

void StripeHandler::setInit(const uint32_t init_time)
{
  init_time_     = init_time;
  readings_      = data_.getSensorsKeyenceData();
  prev_readings_ = data_.getSensorsKeyenceData();
}

uint32_t StripeHandler::getStripeCount() const
{
  return stripe_counter_.value;
}

uint32_t StripeHandler::getFailureCount() const
{
  return num_missed_stripes_;
}

data::nav_t StripeHandler::getStripeDisplacement() const
{
  return stripe_counter_.value * kStripeDist;
}

data::nav_t StripeHandler::getDisplacementOffset(const data::nav_t displacement) const
{
  return displacement - getStripeDisplacement();
}

bool StripeHandler::checkFailure(const data::nav_t displacement)
{
  // Failure if more than one disagreement
  if (getFailureCount() > 1) {
    log_.ERR("NAV", "More than one large IMU/Keyence disagreement, entering kCriticalFailure");
    return true;
  }
  if (getDisplacementOffset(displacement) > kMaxStripeDifference * kStripeDist) {
    log_.ERR("NAV", "IMU distance at least %d * kStripeDist ahead, entering kCriticalFailure",
             kMaxStripeDifference - 1);
    return true;
  }
  return false;
}

void StripeHandler::updateNavData(data::nav_t &displacement, data::nav_t &velocity)
{
  const data::nav_t displacement_offset = getDisplacementOffset(displacement);
  velocity -= displacement_offset * 1e6 / (stripe_counter_.timestamp - init_time_);
  displacement -= displacement_offset;
}

void StripeHandler::queryKeyence(data::nav_t &displacement, data::nav_t &velocity, const bool real)
{
  for (std::size_t i = 0; i < data::Sensors::kNumKeyence; ++i) {
    // Check new readings and ensure new stripe has been hit
    if (prev_readings_[i].count.value == readings_[i].count.value
        || readings_[i].count.timestamp - stripe_counter_.timestamp < 1e5)
      continue;
    stripe_counter_.value++;
    stripe_counter_.timestamp = readings_[i].count.timestamp;
    if (!real) stripe_counter_.timestamp = utils::Timer::getTimeMicros();

    const data::nav_t minimum_uncertainty = kStripeUncertainty;
    const data::nav_t allowed_uncertainty
      = std::max(displacement_uncertainty_, minimum_uncertainty);
    data::nav_t displacement_offset = getDisplacementOffset(displacement);

    const bool single_missed_stripe = displacement_offset > kStripeDist - allowed_uncertainty;
    const bool between_two_stripes  = displacement_offset < kStripeDist + allowed_uncertainty;
    const bool past_half_way_to_next_stripe = displacement_offset > 0.5 * kStripeDist;

    if (single_missed_stripe && between_two_stripes && past_half_way_to_next_stripe) {
      ++stripe_counter_.value;
      displacement_offset -= kStripeDist;
    }
    // Too large disagreement
    if (std::abs(displacement_offset) > 2 * allowed_uncertainty) {
      log_.INFO("NAV", "Displacement change: %.3f, allowed uncertainty: %.3f", displacement_offset,
                allowed_uncertainty);
      ++num_missed_stripes_;
      num_missed_stripes_ += std::floor(std::abs(displacement_offset) / kStripeDist);
    }
    // Lower the uncertainty in velocity
    velocity_uncertainty_
      -= std::abs(displacement_offset * 1e6 / (stripe_counter_.timestamp - init_time_));
    log_.DBG("NAV", "Stripe detected!");
    log_.DBG1("NAV", "Timestamp difference: %d", stripe_counter_.timestamp - init_time_);
    log_.DBG1("NAV", "Timestamp currently:  %d", stripe_counter_.timestamp);

    // Ensure velocity uncertainty is positive
    velocity_uncertainty_ = std::abs(velocity_uncertainty_);
    updateNavData(displacement, velocity);

    break;
  }
  // Update old Keyence readings with current ones
  updateReadings();
}
}  // namespace navigation
}  // namespace hyped
