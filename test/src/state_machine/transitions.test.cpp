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

struct TransitionFunctionality : public ::testing::Test {
  // TODO(miltfra): Disable printing log messages to stdout/stderr
  hyped::utils::Logger log;
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

TEST_F(TransitionFunctionality, handlesEnoughSpaceLeft)
{
  Navigation nav_data;
  int max_displacement;

  static constexpr int num_tests            = 1000;
  static constexpr int min_displacement     = 0;
  static constexpr int min_braking_distance = 0;
  static constexpr int max_braking_distance
    = static_cast<int>(nav_data.run_length - nav_data.braking_buffer);

  std::vector<NavigationType> displacements, braking_distances;
  for (int i = 0; i < num_tests; i++) {
    NavigationType braking_distance = static_cast<NavigationType>(
      min_braking_distance + rand() % (max_braking_distance - min_braking_distance));

    max_displacement = nav_data.run_length - nav_data.braking_buffer - braking_distance;

    NavigationType displacement = static_cast<NavigationType>(
      min_displacement + rand() % (max_displacement - min_displacement));

    braking_distances.push_back(braking_distance);
    displacements.push_back(displacement);
  }

  for (int i = 0; i < num_tests; i++) {
    nav_data.velocity                   = static_cast<NavigationType>(rand());
    nav_data.acceleration               = nav_data.velocity;
    nav_data.emergency_braking_distance = nav_data.velocity;
    nav_data.displacement               = displacements.at(i);
    nav_data.braking_distance           = braking_distances.at(i);
    ASSERT_EQ(false, checkEnteredBrakingZone(log, nav_data))
      << braking_zone_false_positive_error << " " << nav_data.braking_distance << " "
      << nav_data.displacement << " " << max_braking_distance;
  }
}

TEST_F(TransitionFunctionality, handlesNotEnoughSpaceLeft)
{
  Navigation nav_data;
  int min_displacement;

  static constexpr int num_tests            = 1000;
  static constexpr int max_displacement     = nav_data.run_length * 2;
  static constexpr int min_braking_distance = 0;
  static constexpr int max_braking_distance = nav_data.run_length;

  std::vector<NavigationType> displacements, braking_distances;
  for (int i = 0; i < num_tests; i++) {
    NavigationType braking_distance = static_cast<NavigationType>(
      min_braking_distance + rand() % (max_braking_distance - min_braking_distance));

    min_displacement = nav_data.run_length - nav_data.braking_buffer - braking_distance;

    NavigationType displacement = static_cast<NavigationType>(
      min_displacement + rand() % (max_displacement - min_displacement));

    braking_distances.push_back(braking_distance);
    displacements.push_back(displacement);
  }

  for (int i = 0; i < num_tests; i++) {
    nav_data.velocity                   = static_cast<NavigationType>(rand());
    nav_data.acceleration               = nav_data.velocity;
    nav_data.emergency_braking_distance = nav_data.velocity;
    nav_data.displacement               = displacements.at(i);
    nav_data.braking_distance           = braking_distances.at(i);
    ASSERT_EQ(true, checkEnteredBrakingZone(log, nav_data)) << braking_zone_false_negative_error;
  }
}
