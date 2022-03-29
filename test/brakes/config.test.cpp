#include "test.hpp"

#include <memory>

#include <gtest/gtest.h>

#include <brakes/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class BrakesConfigTest : public Test {
};

TEST_F(BrakesConfigTest, prasesConfig)
{
  utils::System &sys = utils::System::getSystem();
  sys.stop();
  auto brakes = std::make_unique<brakes::Main>();
  brakes->start();
  brakes->join();
}

}  // namespace hyped::testing
