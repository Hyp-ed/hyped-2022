#include <fcntl.h>
#include <stdlib.h>

#include <gtest/gtest.h>

#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class Test : public ::testing::Test {
 protected:
  inline static utils::Logger log_ = utils::Logger("TEST", utils::Logger::Level::kNone);
  inline static const std::string kDefaultConfigPath = "configurations/test/default_config.json";
  inline static const char *kDefaultArgs[2]          = {"mock_binary", kDefaultConfigPath.c_str()};
  void initialiseDefaultSystem() { utils::System::parseArgs(2, kDefaultArgs); }
  void SetUp() { initialiseDefaultSystem(); }
  void TearDown() {}
};

}  // namespace hyped::testing
