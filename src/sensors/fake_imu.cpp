#include "fake_imu.hpp"

#include <random>

namespace hyped::sensors {

FakeImu::FakeImu(utils::Logger &log, std::shared_ptr<FakeTrajectory> fake_trajectory,
                 const data::nav_t noise)
    : log_(log),
      data_(data::Data::getInstance()),
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

void FakeImu::getData(data::ImuData &imu_data)
{
  imu_data.operational = true;
  imu_data.acc         = addNoiseToData(getAccurateAcceleration(), noise_);
}

data::NavigationVector FakeImu::addNoiseToData(const data::NavigationVector value,
                                               const data::nav_t noise)
{
  data::NavigationVector temp;
  static std::default_random_engine generator;

  for (int i = 0; i < 3; i++) {
    std::normal_distribution<data::nav_t> distribution(value[i], noise);
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
  return addNoiseToData(value, noise_);
}

}  // namespace hyped::sensors
