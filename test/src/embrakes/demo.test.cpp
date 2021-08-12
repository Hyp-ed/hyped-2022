#include "test.hpp"

#include <memory>

#include <brakes/main.hpp>
#include <gtest/gtest.h>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class BrakesDemoTest : public Test {
};

TEST_F(BrakesDemoTest, handlesSysNotRunning)
{
  utils::System &sys = utils::System::getSystem();
  sys.running_       = false;
  auto brakes        = std::make_unique<brakes::Main>(0, log_);
  brakes->start();
  brakes->join();
}

}  // namespace hyped::testing
