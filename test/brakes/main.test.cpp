#include "test.hpp"

#include <memory>

#include <gtest/gtest.h>

#include <brakes/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class BrakesTest : public Test {
};

TEST_F(BrakesTest, psesConfig)
{
  utils::System &sys = utils::System::getSystem();
  sys.stop();
  auto brakes = std::make_unique<brakes::Main>();
  brakes->start();
  brakes->join();
}

}  // namespace hyped::testing
