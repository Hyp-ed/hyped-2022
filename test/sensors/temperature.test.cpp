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
  utils::System &sys = utils::System::getSystem();
  auto temperature_pin
    = sensors::Main::temperaturePinFromFile(log_, sys.config_.temperature_config_path);
  ASSERT_TRUE(temperature_pin) << "Failed to initialise temperature pin.";
  ASSERT_EQ(temperature_pin, 13) << "Temperature pin value doesn't match config file.";
}
}  // namespace hyped::testing
