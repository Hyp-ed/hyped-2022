#include "test.hpp"

#include <fcntl.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include "data/data.hpp"
#include "gtest/gtest.h"
#include "state_machine/transitions.hpp"
#include "utils/logger.hpp"

namespace hyped::testing {

/**
 * Struct used for testing transition functions. Contains
 *
 * 1. Logger
 * 2. Error messages
 * 3. Constant test size
 * 4. Utility functions
 */
class TransitionFunctionality : public Test {
 protected:
  // ---- Module Status intervals -----------------------

  // Value of the first module status
  static constexpr int kFirst = static_cast<int>(data::ModuleStatus::kCriticalFailure);

  // Value of the first module status that does not indicate an emergency
  static constexpr int kNoEmergencyFirst = static_cast<int>(data::ModuleStatus::kStart);

  // Value of the first module status that indicates initialisation
  static constexpr int kInitFirst = static_cast<int>(data::ModuleStatus::kInit);

  // Value of the first module status that indicates readiness
  static constexpr int kReadyFirst = static_cast<int>(data::ModuleStatus::kReady);

  // Value of the last module status
  static constexpr int kLast = static_cast<int>(data::ModuleStatus::kReady);

  // TODO(miltfra): Disable printing log messages to stdout/stderr

  // ---- Test size -----------

  static constexpr int kTestSize = 1000;

  // ---- Utility function ----

  int randomInRange(int, int);

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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other                      = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status             = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    const bool has_emergency              = state_machine::checkEmergency(
                   log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    enableOutput();
    ASSERT_EQ(has_emergency, false) << "falsely detected emergency";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other                      = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status             = data::ModuleStatus::kCriticalFailure;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    const bool has_emergency              = state_machine::checkEmergency(
                   log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    enableOutput();
    ASSERT_EQ(has_emergency, true) << "failed to detect brake emergency";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other                      = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status             = other;
    nav_data.module_status                = data::ModuleStatus::kCriticalFailure;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    const bool has_emergency              = state_machine::checkEmergency(
                   log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    enableOutput();
    ASSERT_EQ(has_emergency, true) << "failed to detect emergency in Navigation";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other                      = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status             = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = data::ModuleStatus::kCriticalFailure;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    const bool has_emergency              = state_machine::checkEmergency(
                   log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    enableOutput();
    ASSERT_EQ(has_emergency, true) << "failed to detect emergency in Batteries";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other                      = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status             = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = data::ModuleStatus::kCriticalFailure;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    const bool has_emergency              = state_machine::checkEmergency(
                   log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    enableOutput();
    ASSERT_EQ(has_emergency, true) << "failed to detect emergency in Telemetry";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other                      = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status             = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = data::ModuleStatus::kCriticalFailure;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = false;
    const bool has_emergency              = state_machine::checkEmergency(
                   log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    enableOutput();
    ASSERT_EQ(has_emergency, true) << "failed to detect emergency in Sensors";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other                      = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status             = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = data::ModuleStatus::kCriticalFailure;
    telemetry_data.emergency_stop_command = false;
    const bool has_emergency              = state_machine::checkEmergency(
                   log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    enableOutput();
    ASSERT_EQ(has_emergency, true) << "failed to detect emergency in motors";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other                      = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status             = other;
    nav_data.module_status                = other;
    batteries_data.module_status          = other;
    telemetry_data.module_status          = other;
    sensors_data.module_status            = other;
    motors_data.module_status             = other;
    telemetry_data.emergency_stop_command = true;
    const bool has_emergency              = state_machine::checkEmergency(
                   log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    enableOutput();
    ASSERT_EQ(has_emergency, true) << "failed to register stop command";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kInitFirst; i <= kLast; i++) {
    const auto goal              = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status    = goal;
    nav_data.module_status       = goal;
    batteries_data.module_status = goal;
    telemetry_data.module_status = goal;
    sensors_data.module_status   = goal;
    motors_data.module_status    = goal;
    const bool all_initialised   = state_machine::checkModulesInitialised(
        log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_initialised, true) << "failed to detect all modules being initialised";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kInitFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      const auto goal              = static_cast<data::ModuleStatus>(j);
      brakes_data.module_status    = other;
      nav_data.module_status       = goal;
      batteries_data.module_status = goal;
      telemetry_data.module_status = goal;
      sensors_data.module_status   = goal;
      motors_data.module_status    = goal;
      const bool all_initialised   = state_machine::checkModulesInitialised(
          log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << "failed to detect uninitialised brakes";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kInitFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      const auto goal              = static_cast<data::ModuleStatus>(j);
      brakes_data.module_status    = goal;
      nav_data.module_status       = other;
      batteries_data.module_status = goal;
      telemetry_data.module_status = goal;
      sensors_data.module_status   = goal;
      motors_data.module_status    = goal;
      const bool all_initialised   = state_machine::checkModulesInitialised(
          log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << "failed to detect Navigation not being initialised";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kInitFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      const auto goal = static_cast<data::ModuleStatus>(j);

      brakes_data.module_status    = goal;
      nav_data.module_status       = goal;
      batteries_data.module_status = other;
      telemetry_data.module_status = goal;
      sensors_data.module_status   = goal;
      motors_data.module_status    = goal;
      const bool all_initialised   = state_machine::checkModulesInitialised(
          log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << "failed to detect Batteries not being initialised";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kInitFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      const auto goal = static_cast<data::ModuleStatus>(j);

      brakes_data.module_status    = goal;
      nav_data.module_status       = goal;
      batteries_data.module_status = goal;
      telemetry_data.module_status = other;
      sensors_data.module_status   = goal;
      motors_data.module_status    = goal;
      const bool all_initialised   = state_machine::checkModulesInitialised(
          log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << "failed to detect Telemetry not being initialised";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kInitFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      const auto goal = static_cast<data::ModuleStatus>(j);

      brakes_data.module_status    = goal;
      nav_data.module_status       = goal;
      batteries_data.module_status = goal;
      telemetry_data.module_status = goal;
      sensors_data.module_status   = other;
      motors_data.module_status    = goal;
      const bool all_initialised   = state_machine::checkModulesInitialised(
          log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << "failed to detect Sensors not being initialised";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kInitFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      const auto goal = static_cast<data::ModuleStatus>(j);

      brakes_data.module_status    = goal;
      nav_data.module_status       = goal;
      batteries_data.module_status = goal;
      telemetry_data.module_status = goal;
      sensors_data.module_status   = goal;
      motors_data.module_status    = other;
      const bool all_initialised   = state_machine::checkModulesInitialised(
          log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << "failed to detect Motors not being initialise";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  brakes_data.module_status    = data::ModuleStatus::kReady;
  nav_data.module_status       = data::ModuleStatus::kReady;
  batteries_data.module_status = data::ModuleStatus::kReady;
  telemetry_data.module_status = data::ModuleStatus::kReady;
  sensors_data.module_status   = data::ModuleStatus::kReady;
  motors_data.module_status    = data::ModuleStatus::kReady;
  const auto all_ready         = state_machine::checkModulesReady(
            log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
  ASSERT_EQ(all_ready, true) << "failed to detect that all modules are ready";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kReadyFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    brakes_data.module_status    = other;
    nav_data.module_status       = data::ModuleStatus::kReady;
    batteries_data.module_status = data::ModuleStatus::kReady;
    telemetry_data.module_status = data::ModuleStatus::kReady;
    sensors_data.module_status   = data::ModuleStatus::kReady;
    motors_data.module_status    = data::ModuleStatus::kReady;
    const bool all_ready         = state_machine::checkModulesReady(
              log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << "failed to detect Brakes not being ready";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kReadyFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    brakes_data.module_status    = data::ModuleStatus::kReady;
    nav_data.module_status       = other;
    batteries_data.module_status = data::ModuleStatus::kReady;
    telemetry_data.module_status = data::ModuleStatus::kReady;
    sensors_data.module_status   = data::ModuleStatus::kReady;
    motors_data.module_status    = data::ModuleStatus::kReady;
    const bool all_ready         = state_machine::checkModulesReady(
              log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << "failed to detect Navigation not being ready";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kReadyFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    brakes_data.module_status    = data::ModuleStatus::kReady;
    nav_data.module_status       = data::ModuleStatus::kReady;
    batteries_data.module_status = other;
    telemetry_data.module_status = data::ModuleStatus::kReady;
    sensors_data.module_status   = data::ModuleStatus::kReady;
    motors_data.module_status    = data::ModuleStatus::kReady;
    const bool all_ready         = state_machine::checkModulesReady(
              log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << "failed to detect Batteries not being ready";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kReadyFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    brakes_data.module_status    = data::ModuleStatus::kReady;
    nav_data.module_status       = data::ModuleStatus::kReady;
    batteries_data.module_status = data::ModuleStatus::kReady;
    telemetry_data.module_status = other;
    sensors_data.module_status   = data::ModuleStatus::kReady;
    motors_data.module_status    = data::ModuleStatus::kReady;
    const bool all_ready         = state_machine::checkModulesReady(
              log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << "failed to detect Telemetry not being ready";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kReadyFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    brakes_data.module_status    = data::ModuleStatus::kReady;
    nav_data.module_status       = data::ModuleStatus::kReady;
    batteries_data.module_status = data::ModuleStatus::kReady;
    telemetry_data.module_status = data::ModuleStatus::kReady;
    sensors_data.module_status   = other;
    motors_data.module_status    = data::ModuleStatus::kReady;
    const bool all_ready         = state_machine::checkModulesReady(
              log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << "failed to detect Sensors not being ready";
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
  data::EmergencyBrakes brakes_data;
  data::Navigation nav_data;
  data::Batteries batteries_data;
  data::Telemetry telemetry_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kReadyFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    brakes_data.module_status    = data::ModuleStatus::kReady;
    nav_data.module_status       = data::ModuleStatus::kReady;
    batteries_data.module_status = data::ModuleStatus::kReady;
    telemetry_data.module_status = data::ModuleStatus::kReady;
    sensors_data.module_status   = data::ModuleStatus::kReady;
    motors_data.module_status    = other;
    const bool all_ready         = state_machine::checkModulesReady(
              log_, brakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << "failed to detect Motors not being ready";
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
  data::Telemetry telemetry_data;
  static constexpr int kNumCommands = 3;
  for (int i = 0; i < 1 << kNumCommands; i++) {  // 2^num_commands posssible setups
    // extracting bits
    telemetry_data.calibrate_command = static_cast<bool>(i & 1);
    telemetry_data.launch_command    = static_cast<bool>((i >> 1) & 1);
    telemetry_data.shutdown_command  = static_cast<bool>((i >> 2) & 1);
    enableOutput();
    ASSERT_EQ(telemetry_data.calibrate_command,
              state_machine::checkCalibrateCommand(telemetry_data))
      << "failed to react based on calibrate command";
    ASSERT_EQ(telemetry_data.launch_command, state_machine::checkLaunchCommand(telemetry_data))
      << "failed to react based on launch command";
    ASSERT_EQ(telemetry_data.shutdown_command, state_machine::checkShutdownCommand(telemetry_data))
      << "failed to react based on shutdown command";
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
 * Time complexity: O(kTestSize)
 */
TEST_F(TransitionFunctionality, handlesEnoughSpaceLeft)
{
  data::Navigation nav_data;

  constexpr int kMinDisplacement    = 0;
  constexpr int kMinBrakingDistance = 0;
  constexpr int kMaxBrakingDistance
    = static_cast<int>(data::Navigation::kRunLength - data::Navigation::kBrakingBuffer);

  for (int i = 0; i < kTestSize; i++) {
    nav_data.braking_distance
      = static_cast<data::nav_t>(randomInRange(kMinBrakingDistance, kMaxBrakingDistance));
    const data::nav_t max_displacement
      = data::Navigation::kRunLength - data::Navigation::kBrakingBuffer - nav_data.braking_distance;
    nav_data.displacement
      = static_cast<data::nav_t>(randomInRange(kMinDisplacement, max_displacement));
    nav_data.velocity                   = static_cast<data::nav_t>(rand());
    nav_data.acceleration               = nav_data.velocity;
    nav_data.emergency_braking_distance = nav_data.velocity;
    enableOutput();
    ASSERT_EQ(false, state_machine::checkEnteredBrakingZone(log_, nav_data))
      << "falsely detected braking zone";
    disableOutput();
  }
}

/**
 * Tests random integer values in the interval where we expect checkEnteredBrakingZone
 * to return false. We're only checking the rough behaviour as the details may not be
 * relevant for debugging.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(TransitionFunctionality, handlesNotEnoughSpaceLeft)
{
  data::Navigation nav_data;
  constexpr int kMaxDisplacement    = data::Navigation::kRunLength * 2;
  constexpr int kMinBrakingDistance = 0;
  constexpr int kMaxBrakingDistance = data::Navigation::kRunLength;

  for (int i = 0; i < kTestSize; i++) {
    nav_data.braking_distance
      = static_cast<data::nav_t>(randomInRange(kMinBrakingDistance, kMaxBrakingDistance));
    const data::nav_t min_displacement
      = data::Navigation::kRunLength - data::Navigation::kBrakingBuffer - nav_data.braking_distance;
    nav_data.displacement
      = static_cast<data::nav_t>(randomInRange(min_displacement, kMaxDisplacement));
    nav_data.velocity                   = static_cast<data::nav_t>(rand());
    nav_data.acceleration               = static_cast<data::nav_t>(rand());
    nav_data.emergency_braking_distance = static_cast<data::nav_t>(rand());
    enableOutput();
    ASSERT_EQ(true, state_machine::checkEnteredBrakingZone(log_, nav_data))
      << "failed to detect braking zone";
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
 * Time complexity: O(kTestSize^2)
 */
TEST_F(TransitionFunctionality, handlesDisplacementOnEdgeOfBrakingZone)
{
  data::Navigation nav_data;

  constexpr int kMinBrakingDistance = 0;
  constexpr int kMaxBrakingDistance = data::Navigation::kRunLength;
  constexpr data::nav_t kStepSize   = 1.0 / static_cast<data::nav_t>(kTestSize);

  for (int i = 0; i < kTestSize; i++) {
    nav_data.braking_distance
      = static_cast<data::nav_t>(randomInRange(kMinBrakingDistance, kMaxBrakingDistance));
    const data::nav_t critical_displacement
      = data::Navigation::kRunLength - data::Navigation::kBrakingBuffer - nav_data.braking_distance;

    enableOutput();
    for (int j = 0; j < kTestSize; j++) {
      nav_data.displacement = critical_displacement - 0.5 + kStepSize * static_cast<data::nav_t>(j);
      if (j < kTestSize / 2) {
        ASSERT_EQ(false, state_machine::checkEnteredBrakingZone(log_, nav_data))
          << "falsely detected braking zone";
      } else {
        ASSERT_EQ(true, state_machine::checkEnteredBrakingZone(log_, nav_data))
          << "failed to detect braking zone";
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
 * Time complexity: O(kTestSize^2)
 */
TEST_F(TransitionFunctionality, handlesBrakingDistanceOnEdgeOfBrakingZone)
{
  data::Navigation nav_data;
  data::nav_t critical_braking_distance;

  constexpr int kMinDisplacement = 0;
  constexpr int kMaxDisplacement
    = data::Navigation::kRunLength - data::Navigation::kBrakingBuffer - 1;
  constexpr data::nav_t kStepSize = 1.0 / static_cast<data::nav_t>(kTestSize);

  for (int i = 0; i < kTestSize; i++) {
    nav_data.displacement
      = static_cast<data::nav_t>(randomInRange(kMinDisplacement, kMaxDisplacement));
    critical_braking_distance
      = data::Navigation::kRunLength - data::Navigation::kBrakingBuffer - nav_data.displacement;

    enableOutput();
    for (int j = 0; j < kTestSize; j++) {
      nav_data.braking_distance
        = critical_braking_distance - 0.5 + kStepSize * static_cast<data::nav_t>(j);
      if (j < kTestSize / 2) {
        ASSERT_EQ(false, state_machine::checkEnteredBrakingZone(log_, nav_data))
          << "falsely detected braking zone";
      } else {
        ASSERT_EQ(true, state_machine::checkEnteredBrakingZone(log_, nav_data))
          << "failed to detect braking zone";
      }
    }
    disableOutput();
  }
}

/**
 * Ensures that positive velocities result in checkPodStopped
 * returning false.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(TransitionFunctionality, handlesPositiveVelocity)
{
  data::Navigation nav_data;

  constexpr int kMaxVelocity = 100;  // 100 m/s is pretty fast...
  constexpr data::nav_t kStepSize
    = static_cast<data::nav_t>(kMaxVelocity) / static_cast<data::nav_t>(kTestSize);

  for (int i = 1; i <= kTestSize; i++) {
    nav_data.velocity                   = kStepSize * static_cast<data::nav_t>(i);
    nav_data.acceleration               = static_cast<data::nav_t>(rand());
    nav_data.displacement               = static_cast<data::nav_t>(rand());
    nav_data.braking_distance           = static_cast<data::nav_t>(rand());
    nav_data.emergency_braking_distance = static_cast<data::nav_t>(rand());
    enableOutput();
    ASSERT_EQ(false, state_machine::checkPodStopped(log_, nav_data))
      << "falsely detected pod has stopped";
    disableOutput();
  }
}

/**
 * Ensures that nonpositive velocities result in checkPodStopped
 * returning true.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(TransitionFunctionality, handlesNonpositiveVelocity)
{
  data::Navigation nav_data;

  constexpr int kMaxVelocity = 100;  // 100 m/s is pretty fast...
  constexpr data::nav_t kStepSize
    = static_cast<data::nav_t>(kMaxVelocity) / static_cast<data::nav_t>(kTestSize);

  for (int i = 0; i < kTestSize; i++) {
    nav_data.velocity                   = -1.0 * kStepSize * static_cast<data::nav_t>(i);
    nav_data.acceleration               = static_cast<data::nav_t>(rand());
    nav_data.displacement               = static_cast<data::nav_t>(rand());
    nav_data.braking_distance           = static_cast<data::nav_t>(rand());
    nav_data.emergency_braking_distance = static_cast<data::nav_t>(rand());
    enableOutput();
    ASSERT_EQ(true, state_machine::checkPodStopped(log_, nav_data))
      << "failed to detect pod has stopped";
    disableOutput();
  }
}

}  // namespace hyped::testing
