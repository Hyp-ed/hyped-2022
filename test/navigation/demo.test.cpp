#include "test.hpp"

#include <memory>

#include <gtest/gtest.h>

#include <navigation/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class NavigationDemoTest : public Test {
};

TEST_F(NavigationDemoTest, handlesSysNotRunning)
{
  utils::System &sys = utils::System::getSystem();
  sys.stop();
  auto navigation = std::make_unique<navigation::Main>();
  navigation->start();
  navigation->join();
}

}  // namespace hyped::testing
