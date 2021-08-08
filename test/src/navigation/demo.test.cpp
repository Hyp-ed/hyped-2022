#include <gtest/gtest.h>

#include <memory>
#include <navigation/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

#include "../test.hpp"

namespace hyped::testing {

class NavigationDemoTest : public Test {
};

TEST_F(NavigationDemoTest, handlesSysNotRunning)
{
  utils::System &sys = utils::System::getSystem();
  sys.running_       = false;
  auto navigation    = std::make_unique<navigation::Main>(0, log_);
  navigation->start();
  navigation->join();
}

}  // namespace hyped::testing
