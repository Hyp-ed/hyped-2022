#include <gtest/gtest.h>

#include <embrakes/main.hpp>
#include <memory>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

#include "../test.hpp"

namespace hyped::testing {

class EmbrakesDemoTest : public Test {
};

TEST_F(EmbrakesDemoTest, handlesSysNotRunning)
{
  utils::System &sys = utils::System::getSystem();
  sys.running_       = false;
  auto embrakes      = std::make_unique<embrakes::Main>(0, log_);
  embrakes->start();
  embrakes->join();
}

}  // namespace hyped::testing
