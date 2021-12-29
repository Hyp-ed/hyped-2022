#include "test.hpp"

#include <gtest/gtest.h>
#include <sensors/fake_trajectory.hpp>

namespace hyped::testing {

class FakeTrajectoryTest : public Test {
};

TEST_F(FakeTrajectoryTest, parseConfig)
{
  const std::string path     = "configurations/fake_trajectory.json";
  const auto fake_trajectory = sensors::FakeTrajectory::fromFile(log_, path);
  enableOutput();
  ASSERT_TRUE(fake_trajectory);
  const auto &config = fake_trajectory->getConfig();
  ASSERT_FLOAT_EQ(1.0, config.maximum_acceleration);
  ASSERT_FLOAT_EQ(2.0, config.braking_deceleration);
  ASSERT_FLOAT_EQ(0.01, config.cruising_deceleration);
  disableOutput();
}

}  // namespace hyped::testing
