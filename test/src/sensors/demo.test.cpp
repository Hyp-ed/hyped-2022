#include <gtest/gtest.h>

#include <memory>
#include <sensors/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

#include "../test.hpp"

namespace hyped::testing {

using namespace hyped;

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