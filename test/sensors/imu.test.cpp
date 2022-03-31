#include "test.hpp"

#include <gtest/gtest.h>

#include <sensors/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class ImuTest : public Test {
};

TEST_F(ImuTest, parsesConfig)
{
  utils::System &sys  = utils::System::getSystem();
  const auto imu_pins = sensors::Main::imuPinsFromFile(log_, sys.config_.imu_config_path);
  ASSERT_TRUE(imu_pins) << "Failed to initialise IMU pins.";
  std::vector<uint8_t> expected_pins = {22, 27, 47, 86};
  ASSERT_EQ(*imu_pins, expected_pins) << "IMU pin values don't match config file.";
}
}  // namespace hyped::testing
