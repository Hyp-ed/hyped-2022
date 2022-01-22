#include "test.hpp"

#include <gtest/gtest.h>
#include <sensors/imu_manager.hpp>

namespace hyped::testing {

class ImuManagerTest : public Test {
 protected:
  inline static const std::string kOneFaultyConfigPath
    = "configurations/test/fake_imu_one_faulty.json";
  inline static const std::string kFourFaultyConfigPath
    = "configurations/test/fake_imu_four_faulty.json";
};

TEST_F(ImuManagerTest, fakeImusFromFile)
{
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  {
    auto imu_manager_optional
      = sensors::ImuManager::fromFile(log_, kDefaultConfigPath, fake_trajectory);
    enableOutput();
    ASSERT_TRUE(imu_manager_optional);
    auto imu_manager = std::move(*imu_manager_optional);
    ASSERT_TRUE(imu_manager);
    disableOutput();
  }
  {
    auto imu_manager_optional
      = sensors::ImuManager::fromFile(log_, kOneFaultyConfigPath, fake_trajectory);
    enableOutput();
    ASSERT_TRUE(imu_manager_optional);
    auto imu_manager = std::move(*imu_manager_optional);
    ASSERT_TRUE(imu_manager);
    disableOutput();
  }
  {
    auto imu_manager_optional
      = sensors::ImuManager::fromFile(log_, kFourFaultyConfigPath, fake_trajectory);
    enableOutput();
    ASSERT_TRUE(imu_manager_optional);
    auto imu_manager = std::move(*imu_manager_optional);
    ASSERT_TRUE(imu_manager);
    disableOutput();
  }
}

}  // namespace hyped::testing
