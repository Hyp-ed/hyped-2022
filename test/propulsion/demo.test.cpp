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
  sys.stop();
  auto sensors = std::make_unique<propulsion::Main>();
  sensors->start();
  sensors->join();
}

}  // namespace hyped::testing
