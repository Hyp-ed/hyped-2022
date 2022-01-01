#include "fake_keyence.hpp"

#include <random>

namespace hyped::sensors {

FakeKeyence::FakeKeyence(std::shared_ptr<FakeTrajectory> fake_trajectory, const data::nav_t noise)
    : data_(data::Data::getInstance()),
      noise_(noise)
{
  previous_data_.timestamp   = utils::Timer::getTimeMicros();
  previous_data_.value       = 0;  // start stripe count
  previous_data_.operational = true;
}

data::CounterData FakeKeyence::getData()
{
  const auto trajectory   = fake_trajectory_->getTrajectory();
  const auto displacement = addNoiseToDisplacement(trajectory.displacement);
  const auto implied_count
    = static_cast<uint32_t>(displacement / data::Navigation::kWheelCircumfrence);
  if (previous_data_.value < implied_count) {
    previous_data_.timestamp = utils::Timer::getTimeMicros();
    previous_data_.value     = implied_count;
  }
  return previous_data_;
}

data::nav_t FakeKeyence::addNoiseToDisplacement(const data::nav_t displacement) const
{
  static std::default_random_engine generator;
  std::normal_distribution<data::nav_t> distribution(displacement, noise_);
  return distribution(generator);
}

}  // namespace hyped::sensors
