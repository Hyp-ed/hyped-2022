#include "test.hpp"

#include <memory>

#include <gtest/gtest.h>
#include <sensors/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class SensorsDemoTest : public Test {
};

TEST_F(SensorsDemoTest, handlesSysNotRunning)
{
  utils::System &sys = utils::System::getSystem();
  sys.running_       = false;
  auto sensors       = std::make_unique<sensors::Main>(0, log_);
  sensors->start();
  sensors->join();
}

}  // namespace hyped::testing
