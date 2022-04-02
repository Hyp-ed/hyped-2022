#include "test.hpp"

#include <memory>

#include <gtest/gtest.h>

#include <debugging/observer.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class ObserverTest : public Test {
};

TEST_F(ObserverTest, parsesConfig)
{
  const auto observer = debugging::Observer::fromFile(kDefaultConfigPath);
  ASSERT_TRUE(observer);
}

}  // namespace hyped::testing
