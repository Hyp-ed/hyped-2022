/*
 * Author: QA team
 * Organisation: HYPED
 * Date:
 * Description:
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file *
 * except in compliance with the License. You may obtain a copy of the License at
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
  const std::string no_emergency_error        = "Should handle no emergency.";
  const std::string brake_emergency_error     = "Should handle emergency in Embrakes.";
  const std::string nav_emergency_error       = "Should handle emergency in Navigation.";
  const std::string batteries_emergency_error = "Should handle emergency in Batteries.";
  const std::string telemetry_emergency_error = "Should handle emergency in Telemetry.";
  const std::string sensors_emergency_error   = "Should handle emergency in Sensors.";
  const std::string motors_emergency_error    = "Should handle emergency in Motors.";

 protected:
  void SetUp() {}
  void TearDown() {}
};

//--------------------------------------------------------------------------------------
// Emergency
//--------------------------------------------------------------------------------------

TEST_F(TransitionFunctionality, handlesNoEmergency)
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
    embrakes_data.module_status  = other;
    nav_data.module_status       = other;
    batteries_data.module_status = other;
    telemetry_data.module_status = other;
    sensors_data.module_status   = other;
    motors_data.module_status    = other;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, false) << no_emergency_error;
  }
}

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
    embrakes_data.module_status  = ModuleStatus::kCriticalFailure;
    nav_data.module_status       = other;
    batteries_data.module_status = other;
    telemetry_data.module_status = other;
    sensors_data.module_status   = other;
    motors_data.module_status    = other;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << brake_emergency_error;
  }
}

TEST_F(TransitionFunctionality, handlesNavEmergency)
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
    embrakes_data.module_status  = other;
    nav_data.module_status       = ModuleStatus::kCriticalFailure;
    batteries_data.module_status = other;
    telemetry_data.module_status = other;
    sensors_data.module_status   = other;
    motors_data.module_status    = other;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << nav_emergency_error;
  }
}

TEST_F(TransitionFunctionality, handlesBatteriesEmergency)
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
    embrakes_data.module_status  = other;
    nav_data.module_status       = other;
    batteries_data.module_status = ModuleStatus::kCriticalFailure;
    telemetry_data.module_status = other;
    sensors_data.module_status   = other;
    motors_data.module_status    = other;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << batteries_emergency_error;
  }
}

TEST_F(TransitionFunctionality, handlesTelemetryEmergency)
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
    embrakes_data.module_status  = other;
    nav_data.module_status       = other;
    batteries_data.module_status = other;
    telemetry_data.module_status = ModuleStatus::kCriticalFailure;
    sensors_data.module_status   = other;
    motors_data.module_status    = other;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << telemetry_emergency_error;
  }
}

TEST_F(TransitionFunctionality, handlesSensorsEmergency)
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
    embrakes_data.module_status  = other;
    nav_data.module_status       = other;
    batteries_data.module_status = other;
    telemetry_data.module_status = other;
    sensors_data.module_status   = ModuleStatus::kCriticalFailure;
    motors_data.module_status    = other;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << sensors_emergency_error;
  }
}

TEST_F(TransitionFunctionality, handlesMotorsEmergency)
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
    embrakes_data.module_status  = other;
    nav_data.module_status       = other;
    batteries_data.module_status = other;
    telemetry_data.module_status = other;
    sensors_data.module_status   = other;
    motors_data.module_status    = ModuleStatus::kCriticalFailure;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << motors_emergency_error;
  }
}
