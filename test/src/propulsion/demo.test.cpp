#include "test.hpp"

#include <memory>

#include <gtest/gtest.h>
#include <propulsion/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class PropulsionDemoTest : public Test {
};

TEST_F(PropulsionDemoTest, handlesSysNotRunning)
{
  utils::System &sys = utils::System::getSystem();
  sys.running_       = false;
  auto sensors       = std::make_unique<motor_control::Main>(0, log_);
  sensors->start();
  sensors->join();
}

}  // namespace hyped::testing
