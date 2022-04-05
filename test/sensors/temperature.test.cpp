#include "test.hpp"

#include <gtest/gtest.h>

#include <sensors/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class TemperatureTest : public Test {
};

TEST_F(TemperatureTest, parsesConfig)
{
  const auto &sys = utils::System::getSystem();
  const auto temperature_pins
    = sensors::Main::ambientTemperaturePinsFromFile(log_, sys.config_.temperature_config_path);
  ASSERT_TRUE(temperature_pins) << "Failed to initialise temperature pin.";
  std::vector<uint8_t> expected_pins = {13};
  ASSERT_EQ(*temperature_pins, expected_pins) << "Temperature pin value doesn't match config file.";
}
}  // namespace hyped::testing
