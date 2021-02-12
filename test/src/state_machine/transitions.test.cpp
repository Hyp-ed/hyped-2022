/*
 * Author: QA team
 * Organisation: HYPED
 * Date:
 * Description:
 *
 *    Copyright 2019 HYPED
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
#include "state_machine/transitions.hpp"
#include "utils/logger.hpp"

using namespace hyped::data;
using namespace hyped::state_machine;

struct TransitionFunctionality : public ::testing::Test {
  // TODO(miltfra): Disable printing log messages to stdout/stderr
  hyped::utils::Logger log;
  const std::string brake_emergency_error    = "Should handle emergency in Embrakes.";
  const std::string brake_no_emergency_error = "Should handle emergency in Embrakes.";

 protected:
  void SetUp() {}
  void TearDown() {}
};

// Test fixture for 'accelerating' state
TEST_F(TransitionFunctionality, handlesBrakeEmergency)
{
  EmergencyBrakes embrakes_data;

  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool has_emergency;
  for (int i = 0; static_cast<ModuleStatus>(i) != ModuleStatus::kCriticalFailure; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    other                        = static_cast<ModuleStatus>(i);
    nav_data.module_status       = other;
    batteries_data.module_status = other;
    telemetry_data.module_status = other;
    sensors_data.module_status   = other;
    motors_data.module_status    = other;
    // Checking emergency case
    embrakes_data.module_status = ModuleStatus::kCriticalFailure;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << brake_emergency_error;
    // Checking non-emergency case
    embrakes_data.module_status = other;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, false) << brake_no_emergency_error;
  }
}
