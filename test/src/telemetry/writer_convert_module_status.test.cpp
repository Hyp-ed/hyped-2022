/*
 * Author: Robertas Norkus
 * Organisation: HYPED
 * Date: 22/03/2021
 * Description: Tests the conversion of Module Status from Enum to String as required for GUI
 *
 *    Copyright 2021 HYPED
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

#include <string>

#include "data/data.hpp"
#include "gtest/gtest.h"
#include "telemetry/writer.hpp"

using namespace hyped::telemetry;
using namespace hyped::data;

/**
 * Tests the conversion of Module Status from Enum to String as required for GUI
 */
struct WriterConvertModuleStatus : public ::testing::Test {
  // ---- Error messages -------
  const std::string start_status_error            = "Should convert Start status.";
  const std::string init_status_error             = "Should convert Init status.";
  const std::string ready_status_error            = "Should convert Ready status.";
  const std::string critical_failure_status_error = "Should convert Critical Failure status.";

 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(WriterConvertModuleStatus, convertsStartStatus)
{
  std::string convertedModuleStatus = Writer::convertModuleStatus(ModuleStatus::kStart);
  ASSERT_EQ(convertedModuleStatus, "START") << start_status_error;
}

TEST_F(WriterConvertModuleStatus, convertsInitStatus)
{
  std::string convertedModuleStatus = Writer::convertModuleStatus(ModuleStatus::kInit);
  ASSERT_EQ(convertedModuleStatus, "INIT") << init_status_error;
}

TEST_F(WriterConvertModuleStatus, convertsReadyStatus)
{
  std::string convertedModuleStatus = Writer::convertModuleStatus(ModuleStatus::kReady);
  ASSERT_EQ(convertedModuleStatus, "READY") << ready_status_error;
}

TEST_F(WriterConvertModuleStatus, convertsCriticalFailureState)
{
  std::string convertedModuleStatus = Writer::convertModuleStatus(ModuleStatus::kCriticalFailure);
  ASSERT_EQ(convertedModuleStatus, "CRITICAL_FAILURE") << critical_failure_status_error;
}
