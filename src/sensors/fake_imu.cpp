#include "fake_imu.hpp"

#include <math.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <data/data_point.hpp>
#include <utils/math/statistics.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

FakeImu::FakeImu(utils::Logger &log, std::shared_ptr<FakeTrajectory> fake_trajectory,
                 const data::nav_t noise)
    : log_(log),
      data_(data::Data::getInstance()),
      fake_trajectory_(fake_trajectory),
      noise_(noise)
{
}

NavigationVector FakeImu::getAccurateAcceleration()
{
  const auto trajectory = fake_trajectory_->getTrajectory();
  NavigationVector value;
  value[0] = trajectory.acceleration;
  value[1] = 0.0;
  value[2] = 9.8;
  return value;
}

void FakeImu::getData(ImuData *imu)
{
  imu->operational = true;
  imu->acc         = addNoiseToData(getAccurateAcceleration(), noise_);
}

NavigationVector FakeImu::addNoiseToData(const NavigationVector value, const data::nav_t noise)
{
  NavigationVector temp;
  static std::default_random_engine generator;

  for (int i = 0; i < 3; i++) {
    std::normal_distribution<data::nav_t> distribution(value[i], noise);
    temp[i] = distribution(generator);
  }
  return temp;
}

NavigationVector FakeImu::getZeroAcc() const
{
  NavigationVector value;
  value[0] = 0.0;
  value[1] = 0.0;
  value[2] = 9.8;
  return addNoiseToData(value, noise_);
}

}  // namespace hyped::sensors
