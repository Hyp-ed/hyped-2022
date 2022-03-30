#include "test.hpp"

#include <gtest/gtest.h>

#include <sensors/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class TrajectoryTest : public Test {
};

TEST_F(TrajectoryTest, parsesKeyenceConfig)
{
  auto &sys = utils::System::getSystem();
  const auto keyence_pins
    = sensors::Main::keyencePinsFromFile(log_, sys.config_.keyence_config_path);
  sensors::Main::KeyencePins expected_pins = {67, 68};
  ASSERT_TRUE(keyence_pins) << "Failed to initialise keyence pins.";
  ASSERT_EQ(keyence_pins, expected_pins) << "Keyence pin values don't match config file.";
}

TEST_F(TrajectoryTest, parsesImuConfig)
{
  utils::System &sys  = utils::System::getSystem();
  const auto imu_pins = sensors::Main::imuPinsFromFile(log_, sys.config_.keyence_config_path);
  sensors::Main::ImuPins expected_pins = {22, 27, 47, 86};
  ASSERT_TRUE(imu_pins) << "Failed to initialise IMU pins.";
  ASSERT_EQ(imu_pins, expected_pins) << "IMU pin values don't match config file.";
}
}  // namespace hyped::testing
