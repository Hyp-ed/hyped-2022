#include <fcntl.h>
#include <stdlib.h>

#include <gtest/gtest.h>

#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

const std::string kDefaultConfigPath = "configurations/test/default_config.json";

class Test : public ::testing::Test {
 public:
  inline static utils::Logger log_ = utils::Logger("TEST", utils::Logger::Level::kNone);
  void SetUp()
  {
    static const char *args[] = {kDefaultConfigPath.c_str()};
    utils::System::parseArgs(1, args);
  }
  void TearDown() {}
};

}  // namespace hyped::testing
