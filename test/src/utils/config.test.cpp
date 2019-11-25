/*
* Author: QA team
* Organisation: HYPED
* Date:
* Description:
*
*    Copyright 2020 HYPED
*    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
*    except in compliance with the License. You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
*    Unless required by applicable law or agreed to in writing, software distributed under
*    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
*    either express or implied. See the License for the specific language governing permissions and
*    limitations under the License.
*/

/**
 * Description:
 * Test functionality of configuration framework from utils::config->
 *
 * Dependencies:
 * Configuration files have to be defined with the correct data. The top configuration file
 * should be $(ROOT)/configurations/test/config->txt
 */

#include "gtest/gtest.h"
#include "utils/system.hpp"
#include "utils/config.hpp"


struct utils_config: public ::testing::Test {
  hyped::utils::Config* config;

  void SetUp() {
    // construct argument list to pass to System::parseArgs
    char argument0[] = "testing.cpp";
    char argument1[] = "--config=test/config.txt";
    char* args[] = {argument0, argument1};
    hyped::utils::System::parseArgs(2, args);

    // check data configured properly
    config = hyped::utils::System::getSystem().config;
  }
  void TearDown() {}
};

TEST_F(utils_config, test_config_reads_top_config_file)
{
  ASSERT(config->telemetry.IP.compare("192.168.5.3") == 0);
  ASSERT(config->telemetry.Port.compare("7777") == 0);

  ASSERT_EQ(config->sensors.Thermistor, 158);

  ASSERT_EQ(config->statemachine.timeout, 14);
}

TEST_F(utils_config, test_config_reads_subconfig_file)
{
  for (int val : config->embrakes.command) {
    ASSERT_EQ(val, 0);
  }
  for (int val : config->embrakes.button) {
    ASSERT_EQ(val, 1);
  }
}

// TEST_F(configTest, )
