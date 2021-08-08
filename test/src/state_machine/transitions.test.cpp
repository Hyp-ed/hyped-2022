#include "state_machine/transitions.hpp"

#include <fcntl.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include "data/data.hpp"
#include "gtest/gtest.h"
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
  // ---- Module Status intervals -----------------------

  // Value of the first module status
  static constexpr int kFirst = static_cast<int>(ModuleStatus::kCriticalFailure);

  // Value of the first module status that does not indicate an emergency
  static constexpr int kNoEmergencyFirst = static_cast<int>(ModuleStatus::kStart);

  // Value of the first module status that indicates initialisation
  static constexpr int kInitFirst = static_cast<int>(ModuleStatus::kInit);

  // Value of the first module status that indicates readiness
  static constexpr int kReadyFirst = static_cast<int>(ModuleStatus::kReady);

  // Value of the last module status
  static constexpr int kLast = static_cast<int>(ModuleStatus::kReady);

  // TODO(miltfra): Disable printing log messages to stdout/stderr

  // ---- Logger ----------------------------------------

  hyped::utils::Logger log;
  int stdout_f;
  int tmp_stdout_f;

  // ---- Error messages --------------------------------

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
  const std::string batteries_not_ready_error     = "Should handle Batteries not being ready.";
  const std::string telemetry_not_ready_error     = "Should handle Telemetry not being ready.";
  const std::string sensors_not_ready_error       = "Should handle Sensors not being ready.";
  const std::string motors_not_ready_error        = "Should handle Motors not being ready.";
  const std::string calibrate_command_error       = "Should handle calibrate command.";
  const std::string launch_command_error          = "Should handle launch command.";
  const std::string shutdown_command_error        = "Should handle shutdown command.";
  const std::string braking_zone_false_positive_error = "Should handle enough braking space left.";
  const std::string braking_zone_false_negative_error
    = "Should handle not enough braking space left.";
  const std::string pod_stopped_false_positive_error = "Should handle positive velocities.";
  const std::string pod_stopped_false_negative_error = "Should handle nonpositive velocities.";

  // ---- Test size -----------

  static constexpr int TEST_SIZE = 1000;

  // ---- Utility function ----

  int randomInRange(int, int);

  bool output_enabled = true;

  void disableOutput()
  {
    if (!output_enabled) { return; }
    output_enabled = false;
    fflush(stdout);
    stdout_f     = dup(1);
    tmp_stdout_f = open("/dev/null", O_WRONLY);
    dup2(tmp_stdout_f, 1);
    close(tmp_stdout_f);
  }

  void enableOutput()
  {
    if (output_enabled) { return; }
    output_enabled = true;
    fflush(stdout);
    dup2(stdout_f, 1);
    close(stdout_f);
  }

 protected:
  void SetUp() { disableOutput(); }

  void TearDown() { enableOutput(); }
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
  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
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
    enableOutput();
    ASSERT_EQ(has_emergency, false) << no_emergency_error;
    disableOutput();
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
  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
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
    enableOutput();
    ASSERT_EQ(has_emergency, true) << brake_emergency_error;
    disableOutput();
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
  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
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
    enableOutput();
    ASSERT_EQ(has_emergency, true) << nav_emergency_error;
    disableOutput();
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
  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
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
    enableOutput();
    ASSERT_EQ(has_emergency, true) << batteries_emergency_error;
    disableOutput();
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
  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
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
    enableOutput();
    ASSERT_EQ(has_emergency, true) << telemetry_emergency_error;
    disableOutput();
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
  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
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
    enableOutput();
    ASSERT_EQ(has_emergency, true) << sensors_emergency_error;
    disableOutput();
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
  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
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
    enableOutput();
    ASSERT_EQ(has_emergency, true) << motors_emergency_error;
    disableOutput();
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
  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
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
    enableOutput();
    ASSERT_EQ(has_emergency, true) << stop_command_error;
    disableOutput();
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
  ModuleStatus goal;
  for (int i = kInitFirst; i <= kLast; i++) {
    goal                         = static_cast<ModuleStatus>(i);
    embrakes_data.module_status  = goal;
    nav_data.module_status       = goal;
    batteries_data.module_status = goal;
    telemetry_data.module_status = goal;
    sensors_data.module_status   = goal;
    motors_data.module_status    = goal;
    all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                              telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_initialised, true) << all_initialised_error;
  }
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

  ModuleStatus goal, other;
  bool all_initialised;
  for (int i = kFirst; i < kInitFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      goal                         = static_cast<ModuleStatus>(j);
      embrakes_data.module_status  = other;
      nav_data.module_status       = goal;
      batteries_data.module_status = goal;
      telemetry_data.module_status = goal;
      sensors_data.module_status   = goal;
      motors_data.module_status    = goal;
      all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                                telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << brakes_not_initialised_error;
    }
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

  ModuleStatus goal, other;
  bool all_initialised;
  for (int i = kFirst; i < kInitFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      goal                         = static_cast<ModuleStatus>(j);
      embrakes_data.module_status  = goal;
      nav_data.module_status       = other;
      batteries_data.module_status = goal;
      telemetry_data.module_status = goal;
      sensors_data.module_status   = goal;
      motors_data.module_status    = goal;
      all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                                telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << nav_not_initialised_error;
    }
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

  ModuleStatus goal, other;
  bool all_initialised;
  for (int i = kFirst; i < kInitFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      goal = static_cast<ModuleStatus>(j);

      embrakes_data.module_status  = goal;
      nav_data.module_status       = goal;
      batteries_data.module_status = other;
      telemetry_data.module_status = goal;
      sensors_data.module_status   = goal;
      motors_data.module_status    = goal;
      all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                                telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << batteries_not_initialised_error;
    }
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

  ModuleStatus goal, other;
  bool all_initialised;
  for (int i = kFirst; i < kInitFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      goal = static_cast<ModuleStatus>(j);

      embrakes_data.module_status  = goal;
      nav_data.module_status       = goal;
      batteries_data.module_status = goal;
      telemetry_data.module_status = other;
      sensors_data.module_status   = goal;
      motors_data.module_status    = goal;
      all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                                telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << telemetry_not_initialised_error;
    }
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

  ModuleStatus goal, other;
  bool all_initialised;
  for (int i = kFirst; i < kInitFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    embrakes_data.module_status  = ModuleStatus::kInit;
    nav_data.module_status       = ModuleStatus::kInit;
    batteries_data.module_status = ModuleStatus::kInit;
    telemetry_data.module_status = ModuleStatus::kInit;
    sensors_data.module_status   = other;
    motors_data.module_status    = ModuleStatus::kInit;
    all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                              telemetry_data, sensors_data, motors_data);
    enableOutput();
    ASSERT_EQ(all_initialised, false) << sensors_not_initialised_error;
    disableOutput();
    for (int j = kInitFirst; j <= kLast; j++) {
      goal = static_cast<ModuleStatus>(j);

      embrakes_data.module_status  = goal;
      nav_data.module_status       = goal;
      batteries_data.module_status = goal;
      telemetry_data.module_status = goal;
      sensors_data.module_status   = other;
      motors_data.module_status    = goal;
      all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                                telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << sensors_not_initialised_error;
    }
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

  ModuleStatus goal, other;
  bool all_initialised;
  for (int i = kFirst; i < kInitFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      goal = static_cast<ModuleStatus>(j);

      embrakes_data.module_status  = goal;
      nav_data.module_status       = goal;
      batteries_data.module_status = goal;
      telemetry_data.module_status = goal;
      sensors_data.module_status   = goal;
      motors_data.module_status    = other;
      all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                                telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << motors_not_initialised_error;
    }
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
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  bool all_ready;

  embrakes_data.module_status  = ModuleStatus::kReady;
  nav_data.module_status       = ModuleStatus::kReady;
  batteries_data.module_status = ModuleStatus::kReady;
  telemetry_data.module_status = ModuleStatus::kReady;
  sensors_data.module_status   = ModuleStatus::kReady;
  motors_data.module_status    = ModuleStatus::kReady;
  all_ready = checkModulesReady(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                sensors_data, motors_data);
  ASSERT_EQ(all_ready, true) << all_ready_error;
  disableOutput();
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
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_ready;
  for (int i = kFirst; i < kReadyFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    embrakes_data.module_status  = other;
    nav_data.module_status       = ModuleStatus::kReady;
    batteries_data.module_status = ModuleStatus::kReady;
    telemetry_data.module_status = ModuleStatus::kReady;
    sensors_data.module_status   = ModuleStatus::kReady;
    motors_data.module_status    = ModuleStatus::kReady;
    all_ready = checkModulesReady(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                  sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << brakes_not_ready_error;
    disableOutput();
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
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_ready;
  for (int i = kFirst; i < kReadyFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    embrakes_data.module_status  = ModuleStatus::kReady;
    nav_data.module_status       = other;
    batteries_data.module_status = ModuleStatus::kReady;
    telemetry_data.module_status = ModuleStatus::kReady;
    sensors_data.module_status   = ModuleStatus::kReady;
    motors_data.module_status    = ModuleStatus::kReady;
    all_ready = checkModulesReady(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                  sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << nav_not_ready_error;
    disableOutput();
  }
}

/**
 * Makes sure that if navigation is in any state that is not kReady,
 * checkModulesReady never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, handlesBatteriesNotReady)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_ready;
  for (int i = kFirst; i < kReadyFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    embrakes_data.module_status  = ModuleStatus::kReady;
    nav_data.module_status       = ModuleStatus::kReady;
    batteries_data.module_status = other;
    telemetry_data.module_status = ModuleStatus::kReady;
    sensors_data.module_status   = ModuleStatus::kReady;
    motors_data.module_status    = ModuleStatus::kReady;
    all_ready = checkModulesReady(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                  sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << batteries_not_ready_error;
  }
}
/**
 * Makes sure that if navigation is in any state that is not kReady,
 * checkModulesReady never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, handlesTelemetryNotReady)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_ready;
  for (int i = kFirst; i < kReadyFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    embrakes_data.module_status  = ModuleStatus::kReady;
    nav_data.module_status       = ModuleStatus::kReady;
    batteries_data.module_status = ModuleStatus::kReady;
    telemetry_data.module_status = other;
    sensors_data.module_status   = ModuleStatus::kReady;
    motors_data.module_status    = ModuleStatus::kReady;
    all_ready = checkModulesReady(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                  sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << telemetry_not_ready_error;
  }
}

/**
 * Makes sure that if navigation is in any state that is not kReady,
 * checkModulesReady never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, handlesSensorsNotReady)
{
  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_ready;
  for (int i = kFirst; i < kReadyFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    embrakes_data.module_status  = ModuleStatus::kReady;
    nav_data.module_status       = ModuleStatus::kReady;
    batteries_data.module_status = ModuleStatus::kReady;
    telemetry_data.module_status = ModuleStatus::kReady;
    sensors_data.module_status   = other;
    motors_data.module_status    = ModuleStatus::kReady;
    all_ready = checkModulesReady(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                  sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << sensors_not_ready_error;
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
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  ModuleStatus other;
  bool all_ready;
  for (int i = kFirst; i < kReadyFirst; i++) {
    other = static_cast<ModuleStatus>(i);

    embrakes_data.module_status  = ModuleStatus::kReady;
    nav_data.module_status       = ModuleStatus::kReady;
    batteries_data.module_status = ModuleStatus::kReady;
    telemetry_data.module_status = ModuleStatus::kReady;
    sensors_data.module_status   = ModuleStatus::kReady;
    motors_data.module_status    = other;
    all_ready = checkModulesReady(log, embrakes_data, nav_data, batteries_data, telemetry_data,
                                  sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << motors_not_ready_error;
    disableOutput();
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
    enableOutput();
    ASSERT_EQ(telemetry_data.calibrate_command, checkCalibrateCommand(log, telemetry_data))
      << calibrate_command_error;
    ASSERT_EQ(telemetry_data.launch_command, checkLaunchCommand(log, telemetry_data))
      << launch_command_error;
    ASSERT_EQ(telemetry_data.shutdown_command, checkShutdownCommand(log, telemetry_data))
      << shutdown_command_error;
    disableOutput();
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
    = static_cast<int>(Navigation::kRunLength - Navigation::kBrakingBuffer);

  std::vector<nav_t> displacements, braking_distances;
  for (int i = 0; i < TEST_SIZE; i++) {
    nav_data.braking_distance
      = static_cast<nav_t>(randomInRange(min_braking_distance, max_braking_distance));
    max_displacement
      = Navigation::kRunLength - Navigation::kBrakingBuffer - nav_data.braking_distance;
    nav_data.displacement = static_cast<nav_t>(randomInRange(min_displacement, max_displacement));
    nav_data.velocity     = static_cast<nav_t>(rand());
    nav_data.acceleration = nav_data.velocity;
    nav_data.emergency_braking_distance = nav_data.velocity;
    enableOutput();
    ASSERT_EQ(false, checkEnteredBrakingZone(log, nav_data)) << braking_zone_false_positive_error;
    disableOutput();
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

  constexpr int max_displacement     = Navigation::kRunLength * 2;
  constexpr int min_braking_distance = 0;
  constexpr int max_braking_distance = Navigation::kRunLength;

  for (int i = 0; i < TEST_SIZE; i++) {
    nav_data.braking_distance
      = static_cast<nav_t>(randomInRange(min_braking_distance, max_braking_distance));
    min_displacement
      = Navigation::kRunLength - Navigation::kBrakingBuffer - nav_data.braking_distance;
    nav_data.displacement = static_cast<nav_t>(randomInRange(min_displacement, max_displacement));
    nav_data.velocity     = static_cast<nav_t>(rand());
    nav_data.acceleration = static_cast<nav_t>(rand());
    nav_data.emergency_braking_distance = static_cast<nav_t>(rand());
    enableOutput();
    ASSERT_EQ(true, checkEnteredBrakingZone(log, nav_data)) << braking_zone_false_negative_error;
    disableOutput();
  }
}

/**
 * Let d be the first displacement within the precision of nav_t that leads to
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
  nav_t critical_displacement;

  constexpr int min_braking_distance = 0;
  constexpr int max_braking_distance = Navigation::kRunLength;
  constexpr nav_t step_size          = 1.0 / static_cast<nav_t>(TEST_SIZE);

  for (int i = 0; i < TEST_SIZE; i++) {
    nav_data.braking_distance
      = static_cast<nav_t>(randomInRange(min_braking_distance, max_braking_distance));
    critical_displacement
      = Navigation::kRunLength - Navigation::kBrakingBuffer - nav_data.braking_distance;

    enableOutput();
    for (int j = 0; j < TEST_SIZE; j++) {
      nav_data.displacement = critical_displacement - 0.5 + step_size * static_cast<nav_t>(j);
      if (j < TEST_SIZE / 2) {
        ASSERT_EQ(false, checkEnteredBrakingZone(log, nav_data))
          << braking_zone_false_positive_error;
      } else {
        ASSERT_EQ(true, checkEnteredBrakingZone(log, nav_data))
          << braking_zone_false_negative_error;
      }
    }
    disableOutput();
  }
}

/**
 * Let b be the first braking distance within the precision of nav_t that leads to
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
  nav_t critical_braking_distance;

  constexpr int min_displacement = 0;
  constexpr int max_displacement = Navigation::kRunLength - Navigation::kBrakingBuffer - 1;
  constexpr nav_t step_size      = 1.0 / static_cast<nav_t>(TEST_SIZE);

  for (int i = 0; i < TEST_SIZE; i++) {
    nav_data.displacement = static_cast<nav_t>(randomInRange(min_displacement, max_displacement));
    critical_braking_distance
      = Navigation::kRunLength - Navigation::kBrakingBuffer - nav_data.displacement;

    enableOutput();
    for (int j = 0; j < TEST_SIZE; j++) {
      nav_data.braking_distance
        = critical_braking_distance - 0.5 + step_size * static_cast<nav_t>(j);
      if (j < TEST_SIZE / 2) {
        ASSERT_EQ(false, checkEnteredBrakingZone(log, nav_data))
          << braking_zone_false_positive_error;
      } else {
        ASSERT_EQ(true, checkEnteredBrakingZone(log, nav_data))
          << braking_zone_false_negative_error;
      }
    }
    disableOutput();
  }
}

/**
 * Ensures that positive velocities result in checkPodStopped
 * returning false.
 *
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(TransitionFunctionality, handlesPositiveVelocity)
{
  Navigation nav_data;

  constexpr int max_velocity = 100;  // 100 m/s is pretty fast...
  constexpr nav_t step_size  = static_cast<nav_t>(max_velocity) / static_cast<nav_t>(TEST_SIZE);

  for (int i = 1; i <= TEST_SIZE; i++) {
    nav_data.velocity = step_size * static_cast<nav_t>(i);

    nav_data.acceleration               = static_cast<nav_t>(rand());
    nav_data.displacement               = static_cast<nav_t>(rand());
    nav_data.braking_distance           = static_cast<nav_t>(rand());
    nav_data.emergency_braking_distance = static_cast<nav_t>(rand());
    enableOutput();
    ASSERT_EQ(false, checkPodStopped(log, nav_data)) << pod_stopped_false_positive_error;
    disableOutput();
  }
}

/**
 * Ensures that nonpositive velocities result in checkPodStopped
 * returning true.
 *
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(TransitionFunctionality, handlesNonpositiveVelocity)
{
  Navigation nav_data;

  constexpr int max_velocity = 100;  // 100 m/s is pretty fast...
  constexpr nav_t step_size  = static_cast<nav_t>(max_velocity) / static_cast<nav_t>(TEST_SIZE);

  for (int i = 0; i < TEST_SIZE; i++) {
    nav_data.velocity = -1.0 * step_size * static_cast<nav_t>(i);

    nav_data.acceleration               = static_cast<nav_t>(rand());
    nav_data.displacement               = static_cast<nav_t>(rand());
    nav_data.braking_distance           = static_cast<nav_t>(rand());
    nav_data.emergency_braking_distance = static_cast<nav_t>(rand());
    enableOutput();
    ASSERT_EQ(true, checkPodStopped(log, nav_data)) << pod_stopped_false_negative_error;
    disableOutput();
  }
}
