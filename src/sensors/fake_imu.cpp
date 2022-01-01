#include "fake_imu.hpp"

#include <random>

namespace hyped::sensors {

FakeImu::FakeImu(std::shared_ptr<FakeTrajectory> fake_trajectory, const data::nav_t noise)
    : data_(data::Data::getInstance()),
      fake_trajectory_(fake_trajectory),
      noise_(noise)
{
}

data::NavigationVector FakeImu::getAccurateAcceleration()
{
  const auto trajectory = fake_trajectory_->getTrajectory();
  data::NavigationVector value;
  value[0] = trajectory.acceleration;
  value[1] = 0.0;
  value[2] = 9.8;
  return value;
}

data::ImuData FakeImu::getData()
{
  data::ImuData imu_data;
  imu_data.operational = true;
  imu_data.acc         = addNoiseToAcceleration(getAccurateAcceleration());
  return imu_data;
}

data::NavigationVector FakeImu::addNoiseToAcceleration(
  const data::NavigationVector acceleration) const
{
  data::NavigationVector temp;
  static std::default_random_engine generator;

  for (size_t i = 0; i < 3; ++i) {
    std::normal_distribution<data::nav_t> distribution(acceleration[i], noise_);
    temp[i] = distribution(generator);
  }
  return temp;
}

data::NavigationVector FakeImu::getZeroAcc() const
{
  data::NavigationVector value;
  value[0] = 0.0;
  value[1] = 0.0;
  value[2] = 9.8;
  return addNoiseToAcceleration(value);
}

}  // namespace hyped::sensors
