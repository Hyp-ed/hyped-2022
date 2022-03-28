#include "test.hpp"

#include <gtest/gtest.h>

#include <sensors/ambient_pressure.hpp>
#include <sensors/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class AmbientPressureTest : public Test {
};

TEST_F(AmbientPressureTest, parsesConfig)
{
  utils::System &sys = utils::System::getSystem();
  const auto ambient_pressure_pins
    = sensors::Main::ambientPressurePinsFromFile(log_, sys.config_.pressure_config_path);
  ASSERT_TRUE(ambient_pressure_pins) << "Failed to initialise ambient pressure pins.";
  ASSERT_EQ(ambient_pressure_pins->pressure_pin, 160)
    << "Pressure pin value doesn't match config file.";
  ASSERT_EQ(ambient_pressure_pins->temperature_pin, 158)
    << "Temperature pin value doesn't match config file.";
}
}  // namespace hyped::testing
