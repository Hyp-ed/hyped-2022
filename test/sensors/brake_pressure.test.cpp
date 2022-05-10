#include "test.hpp"

#include <gtest/gtest.h>

#include <sensors/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class BrakePressureTest : public Test {
};

TEST_F(BrakePressureTest, parsesConfig)
{
  auto &sys = utils::System::getSystem();
  const auto brake_pressure_pins
    = sensors::Main::brakePressurePinsFromFile(log_, sys.config_.pressure_config_path);
  ASSERT_TRUE(brake_pressure_pins) << "Failed to initialise brake pressure pins.";
  std::vector<uint8_t> expected_pins = {15, 16};
  ASSERT_EQ(*brake_pressure_pins, expected_pins)
    << "Brake Pressure pin values don't match config file.";
}
}  // namespace hyped::testing
