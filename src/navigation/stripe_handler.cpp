#include "stripe_handler.hpp"

#include <algorithm>

namespace hyped {
namespace navigation {

StripeHandler::StripeHandler(utils::Logger &log, data::Data &data, const data::nav_t &displacement_uncertainty, data::nav_t &velocity_uncertainty,
                             const data::nav_t stripe_distance)
    : kStripeDist(stripe_distance),
      log_(log),
      data_(data),
      stripe_counter_(0, 0),
      num_missed_stripes_(0),
      displacement_uncertainty_(displacement_uncertainty),
      velocity_uncertainty_(velocity_uncertainty)
{
}

void StripeHandler::updateNewReadings()
{
  readings_ = data_.getSensorsKeyenceData();
}

void StripeHandler::updatePrevReadings()
{
  prev_readings_ = readings_;
}

void StripeHandler::set_init(uint32_t init_time)
{
  init_time_     = init_time;
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

data::nav_t StripeHandler::getDisplacementOffset(data::nav_t &displacement) const
{
  return displacement - stripe_counter_.value * kStripeDist;
}

bool StripeHandler::checkFailure(data::nav_t displacement)
{
  // Failure if more than one disagreement
  if (num_missed_stripes_ > 1) {
    log_.ERR("NAV", "More than one large IMU/Keyence disagreement, entering kCriticalFailure");
    return true;
  }
  // TODO(Sury): Justify the threshold value of 4 * kStripeDist
  if (getDisplacementOffset(displacement) > 4 * kStripeDist) {
    log_.ERR("NAV", "IMU distance at least 3 * kStripeDist ahead, entering kCriticalFailure.");
    return true;
  }
  return false;
}

void StripeHandler::updateNavData(data::nav_t &displacement, data::nav_t &velocity)
{
  data::nav_t displacement_offset = getDisplacementOffset(displacement);
  velocity -= displacement_offset * 1e6 / (stripe_counter_.timestamp - init_time_);
  displacement -= displacement_offset;
}

void StripeHandler::queryKeyence(data::nav_t &displacement, data::nav_t &velocity, const bool real)
{
  updateNewReadings();

  for (std::size_t i = 0; i < Sensors::kNumKeyence; i++) {
    // Check new readings and ensure new stripe has been hit
    if (prev_readings_[i].count.value == readings_[i].count.value
        || readings_[i].count.timestamp - stripe_counter_.timestamp < 1e5)
      continue;
    stripe_counter_.value++;
    stripe_counter_.timestamp = readings_[i].count.timestamp;
    if (!real) stripe_counter_.timestamp = utils::Timer::getTimeMicros();

    data::nav_t minimum_uncertainty = kStripeDist / 5.;
    data::nav_t allowed_uncertainty = std::max(displacement_uncertainty_, minimum_uncertainty);
    data::nav_t displacement_offset = displacement - stripe_counter_.value * kStripeDist;

    // Allow up to one missed stripe
    if (displacement_offset > kStripeDist - allowed_uncertainty
        && displacement_offset < kStripeDist + allowed_uncertainty
        && displacement > stripe_counter_.value * kStripeDist + 0.5 * kStripeDist) {
      stripe_counter_.value++;
      displacement_offset -= kStripeDist;
    }
    // Too large disagreement
    if (std::abs(displacement_offset) > 2 * allowed_uncertainty) {
      log_.INFO("NAV", "Displacement change: %.3f, allowed uncertainty: %.3f", displacement_offset,
                allowed_uncertainty);
      num_missed_stripes_++;
      num_missed_stripes_ += floor(std::abs(displacement_offset) / kStripeDist);
    }
    // Lower the uncertainty in velocity
    velocity_uncertainty_ -= std::abs(displacement_offset * 1e6 / (stripe_counter_.timestamp - init_time_));
    log_.DBG("NAV", "Stripe detected!");
    log_.DBG1("NAV", "Timestamp difference: %d", stripe_counter_.timestamp - init_time_);
    log_.DBG1("NAV", "Timestamp currently:  %d", stripe_counter_.timestamp);

    // Ensure velocity uncertainty is positive
    velocity_uncertainty_ = std::abs(velocity_uncertainty_);
    updateNavData(displacement, velocity);

    break;
  }
  // Update old keyence readings with current ones
  updatePrevReadings();
}
}  // namespace navigation
}  // namespace hyped

