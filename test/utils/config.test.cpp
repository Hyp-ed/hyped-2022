/**
 * Description:
 * Test functionality of configuration framework from utils::config->
 *
 * Dependencies:
 * Configuration files have to be defined with the correct data. The top configuration file
 * should be $(ROOT)/configurations/test/config->txt
 */

#include <gtest/gtest.h>
#include <utils/config.hpp>
#include <utils/system.hpp>

class utils_config : public ::testing::Test {
 protected:
  hyped::utils::Config *config;

  void SetUp()
  {
    // check data configured properly
    config = hyped::utils::System::getSystem().config;
  }
  void TearDown() {}
};

TEST_F(utils_config, test_config_reads_top_config_file)
{
  ASSERT(config->telemetry.IP.compare("192.168.5.3") == 0);
  ASSERT(config->telemetry.Port.compare("7777") == 0);

  ASSERT_EQ(config->sensors.thermistor, 158);

  ASSERT_EQ(config->statemachine.timeout, 14);
}

TEST_F(utils_config, test_config_reads_subconfig_file)
{
  for (int val : config->brakes.command) {
    ASSERT_EQ(val, 0);
  }
  for (int val : config->brakes.button) {
    ASSERT_EQ(val, 1);
  }
}

// TEST_F(configTest, )
