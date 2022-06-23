#include "test.hpp"

#include <fcntl.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include "data/data.hpp"
#include "demo_state_machine/transitions.hpp"
#include "gtest/gtest.h"
#include "utils/logger.hpp"

namespace hyped::testing {

/**
 * Class used for testing transition functions. Contains
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

  int demoRandomInRange(int, int);
};

int TransitionFunctionality::demoRandomInRange(int min, int max)
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
TEST_F(TransitionFunctionality, demoHandlesNoEmergency)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other             = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status    = other;
    batteries_data.module_status = other;
    sensors_data.module_status   = other;
    motors_data.module_status    = other;
    const bool has_emergency = demo_state_machine::checkEmergency(log_, brakes_data, batteries_data,
                                                                  sensors_data, motors_data);
    ASSERT_EQ(has_emergency, false) << "falsely detected emergency";
  }
}

/*
 * Ensures that if brakes is in kCriticalFailure,
 * checkEmergency always returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, demoHandlesBrakeEmergency)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other             = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status    = data::ModuleStatus::kCriticalFailure;
    batteries_data.module_status = other;
    sensors_data.module_status   = other;
    motors_data.module_status    = other;
    const bool has_emergency = demo_state_machine::checkEmergency(log_, brakes_data, batteries_data,
                                                                  sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << "failed to detect brake emergency";
  }
}

/*
 * Ensures that if batteries is in kCriticalFailure,
 * checkEmergency always returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, demoHandlesBatteriesEmergency)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other             = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status    = other;
    batteries_data.module_status = data::ModuleStatus::kCriticalFailure;
    sensors_data.module_status   = other;
    motors_data.module_status    = other;
    const bool has_emergency = demo_state_machine::checkEmergency(log_, brakes_data, batteries_data,
                                                                  sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << "failed to detect emergency in Batteries";
  }
}

/*
 * Ensures that if sensors is in kCriticalFailure,
 * checkEmergency always returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, demoHandlesSensorsEmergency)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other             = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status    = other;
    batteries_data.module_status = other;
    sensors_data.module_status   = data::ModuleStatus::kCriticalFailure;
    motors_data.module_status    = other;
    const bool has_emergency = demo_state_machine::checkEmergency(log_, brakes_data, batteries_data,
                                                                  sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << "failed to detect emergency in Sensors";
  }
}

/*
 * Ensures that if motors is in kCriticalFailure,
 * checkEmergency always returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, demoHandlesMotorsEmergency)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kNoEmergencyFirst; i <= kLast; i++) {
    // Making sure checkEmergency is unaffected by status of other values.
    const auto other             = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status    = other;
    batteries_data.module_status = other;
    sensors_data.module_status   = other;
    motors_data.module_status    = data::ModuleStatus::kCriticalFailure;
    const bool has_emergency = demo_state_machine::checkEmergency(log_, brakes_data, batteries_data,
                                                                  sensors_data, motors_data);
    ASSERT_EQ(has_emergency, true) << "failed to detect emergency in motors";
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
TEST_F(TransitionFunctionality, demoHandlesAllInitialised)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kInitFirst; i <= kLast; i++) {
    const auto goal              = static_cast<data::ModuleStatus>(i);
    brakes_data.module_status    = goal;
    batteries_data.module_status = goal;
    sensors_data.module_status   = goal;
    motors_data.module_status    = goal;
    const bool all_initialised   = demo_state_machine::checkModulesInitialised(
      log_, brakes_data, batteries_data, sensors_data, motors_data);
    ASSERT_EQ(all_initialised, true) << "failed to detect all modules being initialised";
  }
}

/**
 * Makes sure that if brakes is in any state that is not kInit,
 * checkModulesInitialised never returns true.
 *
 * Time complexity: O(num_states) = O(1)
 */
TEST_F(TransitionFunctionality, demoHandlesBrakesNotInitialised)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kInitFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      const auto goal              = static_cast<data::ModuleStatus>(j);
      brakes_data.module_status    = other;
      batteries_data.module_status = goal;
      sensors_data.module_status   = goal;
      motors_data.module_status    = goal;
      const bool all_initialised   = demo_state_machine::checkModulesInitialised(
        log_, brakes_data, batteries_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << "failed to detect uninitialised brakes";
    }
  }
}

/**
 * Makes sure that if batteries is in any state that is not kInit,
 * checkModulesInitialised never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, demoHandlesBatteriesNotInitialised)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kInitFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      const auto goal = static_cast<data::ModuleStatus>(j);

      brakes_data.module_status    = goal;
      batteries_data.module_status = other;
      sensors_data.module_status   = goal;
      motors_data.module_status    = goal;
      const bool all_initialised   = demo_state_machine::checkModulesInitialised(
        log_, brakes_data, batteries_data, sensors_data, motors_data);
      ASSERT_EQ(all_initialised, false) << "failed to detect Batteries not being initialised";
    }
  }
}

/**
 * Makes sure that if sensors is in any state that is not kInit,
 * checkModulesInitialised never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, demoHandlesSensorsNotInitialised)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kInitFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      const auto goal = static_cast<data::ModuleStatus>(j);

      brakes_data.module_status    = goal;
      batteries_data.module_status = goal;
      sensors_data.module_status   = other;
      motors_data.module_status    = goal;
      const bool all_initialised   = demo_state_machine::checkModulesInitialised(
        log_, brakes_data, batteries_data, sensors_data, motors_data);
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
TEST_F(TransitionFunctionality, demoHandlesMotorsNotInitialised)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kInitFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    for (int j = kInitFirst; j <= kLast; j++) {
      const auto goal = static_cast<data::ModuleStatus>(j);

      brakes_data.module_status    = goal;
      batteries_data.module_status = goal;
      sensors_data.module_status   = goal;
      motors_data.module_status    = other;
      const bool all_initialised   = demo_state_machine::checkModulesInitialised(
        log_, brakes_data, batteries_data, sensors_data, motors_data);
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
TEST_F(TransitionFunctionality, demoHandlesAllReady)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  brakes_data.module_status    = data::ModuleStatus::kReady;
  batteries_data.module_status = data::ModuleStatus::kReady;
  sensors_data.module_status   = data::ModuleStatus::kReady;
  motors_data.module_status    = data::ModuleStatus::kReady;
  const auto all_ready = demo_state_machine::checkModulesReady(log_, brakes_data, batteries_data,
                                                               sensors_data, motors_data);
  ASSERT_EQ(all_ready, true) << "failed to detect that all modules are ready";
}

/**
 * Makes sure that if brakes is in any state that is not kReady,
 * checkModulesReady never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, demoHandlesBrakesNotReady)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kReadyFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    brakes_data.module_status    = other;
    batteries_data.module_status = data::ModuleStatus::kReady;
    sensors_data.module_status   = data::ModuleStatus::kReady;
    motors_data.module_status    = data::ModuleStatus::kReady;
    const bool all_ready = demo_state_machine::checkModulesReady(log_, brakes_data, batteries_data,
                                                                 sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << "failed to detect Brakes not being ready";
  }
}

/**
 * Makes sure that if navigation is in any state that is not kReady,
 * checkModulesReady never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, demoHandlesBatteriesNotReady)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kReadyFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    brakes_data.module_status    = data::ModuleStatus::kReady;
    batteries_data.module_status = other;
    sensors_data.module_status   = data::ModuleStatus::kReady;
    motors_data.module_status    = data::ModuleStatus::kReady;
    const bool all_ready = demo_state_machine::checkModulesReady(log_, brakes_data, batteries_data,
                                                                 sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << "failed to detect Batteries not being ready";
  }
}

/**
 * Makes sure that if navigation is in any state that is not kReady,
 * checkModulesReady never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, demoHandlesSensorsNotReady)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kReadyFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    brakes_data.module_status    = data::ModuleStatus::kReady;
    batteries_data.module_status = data::ModuleStatus::kReady;
    sensors_data.module_status   = other;
    motors_data.module_status    = data::ModuleStatus::kReady;
    const bool all_ready = demo_state_machine::checkModulesReady(log_, brakes_data, batteries_data,
                                                                 sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << "failed to detect Sensors not being ready";
  }
}

/**
 * Makes sure that if motors is in any state that is not kReady,
 * checkModulesReady never returns true.
 *
 * Time complexity: O(num_module_statuses) = O(1)
 */
TEST_F(TransitionFunctionality, demoHandlesMotorsNotReady)
{
  data::Brakes brakes_data;
  data::FullBatteryData batteries_data;
  data::Sensors sensors_data;
  data::Motors motors_data;

  for (int i = kFirst; i < kReadyFirst; i++) {
    const auto other = static_cast<data::ModuleStatus>(i);

    brakes_data.module_status    = data::ModuleStatus::kReady;
    batteries_data.module_status = data::ModuleStatus::kReady;
    sensors_data.module_status   = data::ModuleStatus::kReady;
    motors_data.module_status    = other;
    const bool all_ready = demo_state_machine::checkModulesReady(log_, brakes_data, batteries_data,
                                                                 sensors_data, motors_data);
    ASSERT_EQ(all_ready, false) << "failed to detect Motors not being ready";
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
TEST_F(TransitionFunctionality, demoHandlesAllTelemetryCommands)
{
  data::Telemetry telemetry_data;
  static constexpr int kNumCommands = 3;
  for (int i = 0; i < 1 << kNumCommands; i++) {  // 2^num_commands posssible setups
    // extracting bits
    telemetry_data.calibrate_command = static_cast<bool>(i & 1);
    telemetry_data.launch_command    = static_cast<bool>((i >> 1) & 1);
    telemetry_data.shutdown_command  = static_cast<bool>((i >> 2) & 1);
    telemetry_data.stop_command      = static_cast<bool>((i >> 3) & 1);
    ASSERT_EQ(telemetry_data.calibrate_command,
              demo_state_machine::checkCalibrateCommand(telemetry_data))
      << "failed to react based on calibrate command";
    ASSERT_EQ(telemetry_data.launch_command, demo_state_machine::checkLaunchCommand(telemetry_data))
      << "failed to react based on launch command";
    ASSERT_EQ(telemetry_data.shutdown_command,
              demo_state_machine::checkShutdownCommand(telemetry_data))
      << "failed to react based on shutdown command";
    ASSERT_EQ(telemetry_data.stop_command,
              demo_state_machine::checkStopCommand(log_, telemetry_data))
      << "failed to react based on stop command";
  }
}

}  // namespace hyped::testing
