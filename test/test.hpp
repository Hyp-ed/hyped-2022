#include <fcntl.h>
#include <stdlib.h>

#include <gtest/gtest.h>

#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

const std::string kDefaultConfigPath = "configurations/test/default_config.json";

class Test : public ::testing::Test {
 protected:
  // File descriptors for the original and the temporary standard output stream to avoid logging
  // during testing.
  int stdout_f_;
  int tmp_stdout_f_;

  bool output_enabled_ = true;

  /**
   * Reroutes stdout into /dev/null and saves the original output stream to be restored later.
   */
  void disableOutput()
  {
    if (!output_enabled_) { return; }
    output_enabled_ = false;
    fflush(stdout);
    stdout_f_     = dup(1);
    tmp_stdout_f_ = open("/dev/null", O_WRONLY);
    dup2(tmp_stdout_f_, 1);
    close(tmp_stdout_f_);
  }

  /**
   *  Restores the original output stream to stdout.
   */
  void enableOutput()
  {
    if (output_enabled_) { return; }
    output_enabled_ = true;
    fflush(stdout);
    dup2(stdout_f_, 1);
    close(stdout_f_);
  }

 public:
  inline static utils::Logger log_ = utils::Logger("TEST");
  void SetUp()
  {
    disableOutput();
    static const char *args[] = {kDefaultConfigPath.c_str()};
    utils::System::parseArgs(1, args);
  }
  void TearDown() { enableOutput(); }
};

}  // namespace hyped::testing
