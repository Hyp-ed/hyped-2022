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

#include <stdlib.h>

#include <string>
#include <vector>

#include "data/data.hpp"
#include "gtest/gtest.h"
#include "state_machine/transitions.hpp"
#include "utils/logger.hpp"

using namespace hyped::data;
using namespace hyped::state_machine;

/**
 * Struct used for testing transition functions. Contains
 *
 * 1. Logger
 * 2. Error messages
 * 3. Constant test size
 * 4. Utility functions
 */
struct TransitionFunctionality : public ::testing::Test {
  // TODO(miltfra): Disable printing log messages to stdout/stderr

  // ---- Logger ---------------

  hyped::utils::Logger log;

  // ---- Error messages -------

  const std::string no_emergency_error           = "Should handle no emergency.";
  const std::string brake_emergency_error        = "Should handle emergency in Embrakes.";
  const std::string nav_emergency_error          = "Should handle emergency in Navigation.";
  const std::string batteries_emergency_error    = "Should handle emergency in Batteries.";
  const std::string telemetry_emergency_error    = "Should handle emergency in Telemetry.";
  const std::string sensors_emergency_error      = "Should handle emergency in Sensors.";
  const std::string motors_emergency_error       = "Should handle emergency in Motors.";
  const std::string stop_command_error           = "Should handle stop command.";
  const std::string all_initialised_error        = "Should handle all modules being initialised.";
  const std::string brakes_not_initialised_error = "Should handle Brakes not being initialised.";
  const std::string nav_not_initialised_error = "Should handle Navigation not being initialised.";
  const std::string batteries_not_initialised_error
    = "Should handle Batteries not being initialised.";
  const std::string telemetry_not_initialised_error
    = "Should handle Telemetry not being initialised.";
  const std::string sensors_not_initialised_error = "Should handle Sensors not being initialised.";
  const std::string motors_not_initialised_error  = "Should handle Motors not being initialised.";
  const std::string all_ready_error               = "Should handle all modules being ready.";
  const std::string brakes_not_ready_error        = "Should handle Brakes not being ready.";
  const std::string nav_not_ready_error           = "Should handle Navigation not being ready.";
  const std::string motors_not_ready_error        = "Should handle Motors not being ready.";
  const std::string calibrate_command_error       = "Should handle calibrate command.";
  const std::string launch_command_error          = "Should handle launch command.";
  const std::string shutdown_command_error        = "Should handle shutdown command.";
  const std::string braking_zone_false_positive_error = "Should handle enough braking space left.";
  const std::string braking_zone_false_negative_error
    = "Should handle not enough braking space left.";

  // ---- Test size -----------

  static constexpr int TEST_SIZE = 1000;

  // ---- Utility function ----

  int randomInRange(int, int);

 protected:
  void SetUp() {}
  void TearDown() {}
};

int TransitionFunctionality::randomInRange(int min, int max)
{
  return min + rand() % (max - min);
}

//--------------------------------------------------------------------------------------
// Emergency
//--------------------------------------------------------------------------------------

/**
 * Ensures that if no module reports an emergency and no
 * emergency stop command has been received, checkEmergency
 * does not return true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
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
    other                                 = static_cast<ModuleStatus>(i);
    embrakes_data.module_status           = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, false) << no_emergency_error;
  }
}

/*
 * Ensures that if brakes is in kCriticalFailure,
 * checkEmergency always returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
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
    other                                 = static_cast<ModuleStatus>(i);
    embrakes_data.module_status           = ModuleStatus::kCriticalFailure;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << brake_emergency_error;
  }
}

/*
 * Ensures that if navigation is in kCriticalFailure,
 * checkEmergency always returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
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
    other                                 = static_cast<ModuleStatus>(i);
    embrakes_data.module_status           = other;
    nav_data.module_status                = ModuleStatus::kCriticalFailure;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << nav_emergency_error;
  }
}

/*
 * Ensures that if batteries is in kCriticalFailure,
 * checkEmergency always returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
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
    other                                 = static_cast<ModuleStatus>(i);
    embrakes_data.module_status           = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = ModuleStatus::kCriticalFailure;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << batteries_emergency_error;
  }
}

/*
 * Ensures that if telemetry is in kCriticalFailure,
 * checkEmergency always returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
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
    other                                 = static_cast<ModuleStatus>(i);
    embrakes_data.module_status           = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = ModuleStatus::kCriticalFailure;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << telemetry_emergency_error;
  }
}

/*
 * Ensures that if sensors is in kCriticalFailure,
 * checkEmergency always returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
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
    other                                 = static_cast<ModuleStatus>(i);
    embrakes_data.module_status           = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = ModuleStatus::kCriticalFailure;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << sensors_emergency_error;
  }
}

/*
 * Ensures that if motors is in kCriticalFailure,
 * checkEmergency always returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
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
    other                                 = static_cast<ModuleStatus>(i);
    embrakes_data.module_status           = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = ModuleStatus::kCriticalFailure;
    telemetry_data.emergency_stop_command = false;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << motors_emergency_error;
  }
}

/*
 * Ensures that if an emergency stop command has been received,
 * checkEmergency always returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, handlesStopCommand)
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
    other                                 = static_cast<ModuleStatus>(i);
    embrakes_data.module_status           = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = true;
    has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                   sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << stop_command_error;
  }
}

//--------------------------------------------------------------------------------------
// Module Status
//--------------------------------------------------------------------------------------

/**
 * Makes sure that if all modules are in kInit,
 * checkModulesInitialised returns true.
 *
 * Time complexity: O(1)
 */
TEST_F(TransitionFunctionality, handlesAllInitialised)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  bool all_initialised;

  embrakes_data.module_status  = ModuleStatus::kInit;
  nav_data.module_status       = ModuleStatus::kInit;
  batteries_data.module_status = ModuleStatus::kInit;
  telemetry_data.module_status = ModuleStatus::kInit;
  sensors_data.module_status   = ModuleStatus::kInit;
  motors_data.module_status    = ModuleStatus::kInit;
  all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                            telemetry_data, sensors_data, motors_data);
  ASSERT_EQ(all_initialised, true) << all_initialised_error;
}

/**
 * Makes sure that if brakes is in any state that is not kInit,
 * checkModulesInitialised never returns true.
 *
 * Time complexity: O(num_states) = O(1)
 */
TEST_F(TransitionFunctionality, handlesBrakesNotInitialised)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_initialised;
  for (int i = 0; i != static_cast<int>(ModuleStatus::kCriticalFailure) + 1; i++) {
    // Making sure checkModulesInitialised is unaffected by actual non-init state.
    other = static_cast<ModuleStatus>(i);
    if (other == ModuleStatus::kInit) { continue; }

    embrakes_data.module_status  = other;
    nav_data.module_status       = ModuleStatus::kInit;
    batteries_data.module_status = ModuleStatus::kInit;
    telemetry_data.module_status = ModuleStatus::kInit;
    sensors_data.module_status   = ModuleStatus::kInit;
    motors_data.module_status    = ModuleStatus::kInit;
    all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                              telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_initialised, false) << brakes_not_initialised_error;
  }
}

/**
 * Makes sure that if navigation is in any state that is not kInit,
 * checkModulesReady never returns true.
 *
 * Time complexity: O(num_states) = O(1)
 */
TEST_F(TransitionFunctionality, handlesNavigationNotInitialised)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_initialised;
  for (int i = 0; i != static_cast<int>(ModuleStatus::kCriticalFailure) + 1; i++) {
    // Making sure checkModulesInitialised is unaffected by actual non-init state.
    other = static_cast<ModuleStatus>(i);
    if (other == ModuleStatus::kInit) { continue; }

    embrakes_data.module_status  = ModuleStatus::kInit;
    nav_data.module_status       = other;
    batteries_data.module_status = ModuleStatus::kInit;
    telemetry_data.module_status = ModuleStatus::kInit;
    sensors_data.module_status   = ModuleStatus::kInit;
    motors_data.module_status    = ModuleStatus::kInit;
    all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                              telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_initialised, false) << nav_not_initialised_error;
  }
}

/**
 * Makes sure that if batteries is in any state that is not kInit,
 * checkModulesInitialised never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, handlesBatteriesNotInitialised)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_initialised;
  for (int i = 0; i != static_cast<int>(ModuleStatus::kCriticalFailure) + 1; i++) {
    // Making sure checkModulesInitialised is unaffected by actual non-init state.
    other = static_cast<ModuleStatus>(i);
    if (other == ModuleStatus::kInit) { continue; }

    embrakes_data.module_status  = ModuleStatus::kInit;
    nav_data.module_status       = ModuleStatus::kInit;
    batteries_data.module_status = other;
    telemetry_data.module_status = ModuleStatus::kInit;
    sensors_data.module_status   = ModuleStatus::kInit;
    motors_data.module_status    = ModuleStatus::kInit;
    all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                              telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_initialised, false) << batteries_not_initialised_error;
  }
}

/**
 * Makes sure that if telemetry is in any state that is not kInit,
 * checkModulesInitialised never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, handlesTelemetryNotInitialised)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_initialised;
  for (int i = 0; i != static_cast<int>(ModuleStatus::kCriticalFailure) + 1; i++) {
    // Making sure checkModulesInitialised is unaffected by actual non-init state.
    other = static_cast<ModuleStatus>(i);
    if (other == ModuleStatus::kInit) { continue; }

    embrakes_data.module_status  = ModuleStatus::kInit;
    nav_data.module_status       = ModuleStatus::kInit;
    batteries_data.module_status = ModuleStatus::kInit;
    telemetry_data.module_status = other;
    sensors_data.module_status   = ModuleStatus::kInit;
    motors_data.module_status    = ModuleStatus::kInit;
    all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                              telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_initialised, false) << telemetry_not_initialised_error;
  }
}

/**
 * Makes sure that if sensors is in any state that is not kInit,
 * checkModulesInitialised never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, handlesSensorsNotInitialised)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_initialised;
  for (int i = 0; i != static_cast<int>(ModuleStatus::kCriticalFailure) + 1; i++) {
    // Making sure checkModulesInitialised is unaffected by actual non-init state.
    other = static_cast<ModuleStatus>(i);
    if (other == ModuleStatus::kInit) { continue; }

    embrakes_data.module_status  = ModuleStatus::kInit;
    nav_data.module_status       = ModuleStatus::kInit;
    batteries_data.module_status = ModuleStatus::kInit;
    telemetry_data.module_status = ModuleStatus::kInit;
    sensors_data.module_status   = other;
    motors_data.module_status    = ModuleStatus::kInit;
    all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                              telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_initialised, false) << sensors_not_initialised_error;
  }
}

/**
 * Makes sure that if motors is in any state that is not kInit,
 * checkModulesInitialised never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, handlesMotorsNotInitialised)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_initialised;
  for (int i = 0; i != static_cast<int>(ModuleStatus::kCriticalFailure) + 1; i++) {
    // Making sure checkModulesInitialised is unaffected by actual non-init state.
    other = static_cast<ModuleStatus>(i);
    if (other == ModuleStatus::kInit) { continue; }

    embrakes_data.module_status  = ModuleStatus::kInit;
    nav_data.module_status       = ModuleStatus::kInit;
    batteries_data.module_status = ModuleStatus::kInit;
    telemetry_data.module_status = ModuleStatus::kInit;
    sensors_data.module_status   = ModuleStatus::kInit;
    motors_data.module_status    = other;
    all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                              telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_initialised, false) << motors_not_initialised_error;
  }
}

/**
 * Makes sure that if all modules are in kReady,
 * checkModulesReady returns true.
 *
 * Time complexity: O(1)
 */
TEST_F(TransitionFunctionality, handlesAllReady)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Motors motors_data;

  bool all_ready;

  embrakes_data.module_status = ModuleStatus::kReady;
  nav_data.module_status      = ModuleStatus::kReady;
  motors_data.module_status   = ModuleStatus::kReady;
  all_ready                   = checkModulesReady(log, embrakes_data, nav_data, motors_data);
  ASSERT_EQ(all_ready, true) << all_ready_error;
}

/**
 * Makes sure that if brakes is in any state that is not kReady,
 * checkModulesReady never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, handlesBrakesNotReady)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_ready;
  for (int i = 0; i != static_cast<int>(ModuleStatus::kCriticalFailure) + 1; i++) {
    other = static_cast<ModuleStatus>(i);
    if (other == ModuleStatus::kReady) { continue; }

    embrakes_data.module_status = other;
    nav_data.module_status      = ModuleStatus::kReady;
    motors_data.module_status   = ModuleStatus::kReady;
    all_ready                   = checkModulesReady(log, embrakes_data, nav_data, motors_data);
    ASSERT_EQ(all_ready, false) << brakes_not_ready_error;
  }
}

/**
 * Makes sure that if navigation is in any state that is not kReady,
 * checkModulesReady never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, handlesNavigationNotReady)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_ready;
  for (int i = 0; i != static_cast<int>(ModuleStatus::kCriticalFailure) + 1; i++) {
    other = static_cast<ModuleStatus>(i);
    if (other == ModuleStatus::kReady) { continue; }

    embrakes_data.module_status = ModuleStatus::kReady;
    nav_data.module_status      = other;
    motors_data.module_status   = ModuleStatus::kReady;
    all_ready                   = checkModulesReady(log, embrakes_data, nav_data, motors_data);
    ASSERT_EQ(all_ready, false) << nav_not_ready_error;
  }
}

/**
 * Makes sure that if motors is in any state that is not kReady,
 * checkModulesReady never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, handlesMotorsNotReady)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_ready;
  for (int i = 0; i != static_cast<int>(ModuleStatus::kCriticalFailure) + 1; i++) {
    other = static_cast<ModuleStatus>(i);
    if (other == ModuleStatus::kReady) { continue; }

    embrakes_data.module_status = ModuleStatus::kReady;
    nav_data.module_status      = ModuleStatus::kReady;
    motors_data.module_status   = other;
    all_ready                   = checkModulesReady(log, embrakes_data, nav_data, motors_data);
    ASSERT_EQ(all_ready, false) << motors_not_ready_error;
  }
}

//--------------------------------------------------------------------------------------
// Telemetry Commands
//--------------------------------------------------------------------------------------

/**
 * Cycles through all command setups and makes sure the transition checks behave
 * as intended.
 *
 * Time complexity: O(2^num_commands) = O(1)
 */
TEST_F(TransitionFunctionality, handlesAllTelemetryCommands)
{
  Telemetry telemetry_data;
  static constexpr int num_commands = 3;
  for (int i = 0; i < 1 << num_commands; i++) {  // 2^num_commands posssible setups
    // extracting bits
    telemetry_data.calibrate_command = static_cast<bool>(i & 1);
    telemetry_data.launch_command    = static_cast<bool>((i >> 1) & 1);
    telemetry_data.shutdown_command  = static_cast<bool>((i >> 2) & 1);
    ASSERT_EQ(telemetry_data.calibrate_command, checkCalibrateCommand(log, telemetry_data))
      << calibrate_command_error;
    ASSERT_EQ(telemetry_data.launch_command, checkLaunchCommand(log, telemetry_data))
      << launch_command_error;
    ASSERT_EQ(telemetry_data.shutdown_command, checkShutdownCommand(log, telemetry_data))
      << shutdown_command_error;
  }
}

//--------------------------------------------------------------------------------------
// Navigation Data Events
//--------------------------------------------------------------------------------------

/**
 * Tests random integer values in the interval where we expect checkEnteredBrakingZone
 * to return true. We're only checking the rough behaviour as the details may not be
 * relevant for debugging.
 *
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(TransitionFunctionality, handlesEnoughSpaceLeft)
{
  Navigation nav_data;
  int max_displacement;

  constexpr int min_displacement     = 0;
  constexpr int min_braking_distance = 0;
  constexpr int max_braking_distance
    = static_cast<int>(nav_data.run_length - nav_data.braking_buffer);

  std::vector<NavigationType> displacements, braking_distances;
  for (int i = 0; i < TEST_SIZE; i++) {
    nav_data.braking_distance
      = static_cast<NavigationType>(randomInRange(min_braking_distance, max_braking_distance));
    max_displacement = nav_data.run_length - nav_data.braking_buffer - nav_data.braking_distance;
    nav_data.displacement
      = static_cast<NavigationType>(randomInRange(min_displacement, max_displacement));
    nav_data.velocity                   = static_cast<NavigationType>(rand());
    nav_data.acceleration               = nav_data.velocity;
    nav_data.emergency_braking_distance = nav_data.velocity;

    ASSERT_EQ(false, checkEnteredBrakingZone(log, nav_data)) << braking_zone_false_positive_error;
  }
}

/**
 * Tests random integer values in the interval where we expect checkEnteredBrakingZone
 * to return false. We're only checking the rough behaviour as the details may not be
 * relevant for debugging.
 *
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(TransitionFunctionality, handlesNotEnoughSpaceLeft)
{
  Navigation nav_data;
  int min_displacement;

  constexpr int max_displacement     = nav_data.run_length * 2;
  constexpr int min_braking_distance = 0;
  constexpr int max_braking_distance = nav_data.run_length;

  for (int i = 0; i < TEST_SIZE; i++) {
    nav_data.braking_distance
      = static_cast<NavigationType>(randomInRange(min_braking_distance, max_braking_distance));
    min_displacement = nav_data.run_length - nav_data.braking_buffer - nav_data.braking_distance;
    nav_data.displacement
      = static_cast<NavigationType>(randomInRange(min_displacement, max_displacement));
    nav_data.velocity                   = static_cast<NavigationType>(rand());
    nav_data.acceleration               = static_cast<NavigationType>(rand());
    nav_data.emergency_braking_distance = static_cast<NavigationType>(rand());

    ASSERT_EQ(true, checkEnteredBrakingZone(log, nav_data)) << braking_zone_false_negative_error;
  }
}

/**
 * Let d be the first displacement within the precision of NavigationType that leads to
 * checkEnteredBrakingZone returning true. Then this test aims to test values in the interval
 * [d-0.5,d+0.5) to make sure that checkEnteredBrakingZone behaves correctly in this edge case.
 *
 * This is a rather expensive test and may not be required to run during debugging.
 *
 * Time complexity: O(TEST_SIZE^2)
 */
TEST_F(TransitionFunctionality, handlesDisplacementOnEdgeOfBrakingZone)
{
  Navigation nav_data;
  NavigationType critical_displacement;

  constexpr int min_braking_distance = 0;
  constexpr int max_braking_distance = nav_data.run_length;
  constexpr NavigationType step_size = 1.0 / static_cast<NavigationType>(TEST_SIZE);

  for (int i = 0; i < TEST_SIZE; i++) {
    nav_data.braking_distance = static_cast<NavigationType>(
      min_braking_distance + rand() % (max_braking_distance - min_braking_distance));
    critical_displacement
      = nav_data.run_length - nav_data.braking_buffer - nav_data.braking_distance;

    for (int j = 0; j < TEST_SIZE; j++) {
      nav_data.displacement
        = critical_displacement - 0.5 + step_size * static_cast<NavigationType>(j);
      if (j < TEST_SIZE / 2) {
        ASSERT_EQ(false, checkEnteredBrakingZone(log, nav_data))
          << braking_zone_false_positive_error;
      } else {
        ASSERT_EQ(true, checkEnteredBrakingZone(log, nav_data))
          << braking_zone_false_negative_error;
      }
    }
  }
}

/**
 * Let b be the first braking distance within the precision of NavigationType that leads to
 * checkEnteredBrakingZone returning true. Then this test aims to test values in the interval
 * [b-0.5,b+0.5) to make sure that checkEnteredBrakingZone behaves correctly in this edge case.
 *
 * This is a rather expensive test and may not be required to run during debugging.
 *
 * Time complexity: O(TEST_SIZE^2)
 */
TEST_F(TransitionFunctionality, handlesBrakingDistanceOnEdgeOfBrakingZone)
{
  Navigation nav_data;
  NavigationType critical_braking_distance;

  constexpr int min_displacement     = 0;
  constexpr int max_displacement     = nav_data.run_length - nav_data.braking_buffer - 1;
  constexpr NavigationType step_size = 1.0 / static_cast<NavigationType>(TEST_SIZE);

  for (int i = 0; i < TEST_SIZE; i++) {
    nav_data.displacement
      = static_cast<NavigationType>(randomInRange(min_displacement, max_displacement));
    critical_braking_distance
      = nav_data.run_length - nav_data.braking_buffer - nav_data.displacement;

    for (int j = 0; j < TEST_SIZE; j++) {
      nav_data.braking_distance
        = critical_braking_distance - 0.5 + step_size * static_cast<NavigationType>(j);
      if (j < TEST_SIZE / 2) {
        ASSERT_EQ(false, checkEnteredBrakingZone(log, nav_data))
          << braking_zone_false_positive_error;
      } else {
        ASSERT_EQ(true, checkEnteredBrakingZone(log, nav_data))
          << braking_zone_false_negative_error;
      }
    }
  }
}
