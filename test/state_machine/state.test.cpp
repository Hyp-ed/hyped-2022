#include "randomiser.hpp"
#include "test.hpp"

#include <fcntl.h>
#include <stdlib.h>

#include <random>
#include <string>
#include <vector>

#include <data/data.hpp>
#include <gtest/gtest.h>
#include <state_machine/state.hpp>
#include <state_machine/transitions.hpp>
#include <utils/logger.hpp>

namespace hyped::testing {

/**
 * Class used for testing state behaviour. Contains
 *
 * 1. Variables
 * 1. Logger
 * 2. Error messages
 * 3. Constant test size
 */

class StateTest : public hyped::testing::Test {
  data::Data &data_ = data::Data::getInstance();

 protected:
  data::EmergencyBrakes brakes_data_;
  data::Navigation nav_data_;
  data::Batteries batteries_data_;
  data::Telemetry telemetry_data_;
  data::Sensors sensors_data_;
  data::Motors motors_data_;

  // ---- Test size -----------

  static constexpr int kTestSize = 1000;

  // ---- Methods -------------

  /**
   * Generates random values for all the entries in data and overwrites them.
   */
  void randomiseData()
  {
    Randomiser::randomiseBrakes(brakes_data_);
    Randomiser::randomiseNavigation(nav_data_);
    Randomiser::randomiseTelemetry(telemetry_data_);
    Randomiser::randomiseMotors(motors_data_);
    Randomiser::randomiseSensorsData(sensors_data_);
    Randomiser::randomiseBatteriesData(batteries_data_);

    data_.setEmergencyBrakesData(brakes_data_);
    data_.setNavigationData(nav_data_);
    data_.setTelemetryData(telemetry_data_);
    data_.setMotorData(motors_data_);
    data_.setSensorsData(sensors_data_);
    data_.setBatteriesData(batteries_data_);
  }

  void SetUp() { disableOutput(); }

  void TearDown() { enableOutput(); }
};

//---------------------------------------------------------------------------
// Idle Tests
//---------------------------------------------------------------------------

/**
 * Testing Idle behaviour with respect to data
 */
struct IdleTest : public StateTest {
  state_machine::Idle *state = state_machine::Idle::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 */
TEST_F(IdleTest, handlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    enableOutput();
    if (has_emergency) {
      ASSERT_EQ(new_state, state_machine::FailureStopped::getInstance())
        << "failed to enter FailureStopped from Idle";
    } else {
      ASSERT_NE(new_state, state_machine::FailureStopped::getInstance())
        << "falsely entered FailureStopped from Idle";
    }
    disableOutput();
  }
}

/**
 * Ensures that if no module reports an emergency and if every module
 * is initialised, the state changes to the PreCalibrating state.
 */
TEST_F(IdleTest, handlesAllInitialised)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool all_initialised
        = state_machine::checkModulesInitialised(log_, brakes_data_, nav_data_, batteries_data_,
                                                 telemetry_data_, sensors_data_, motors_data_);
      const auto new_state = state->checkTransition(log_);

      enableOutput();
      if (all_initialised) {
        ASSERT_EQ(new_state, state_machine::PreCalibrating::getInstance())
          << "failed to enter PreCalibrating from Idle";
      } else {
        ASSERT_NE(new_state, state_machine::PreCalibrating::getInstance())
          << "falsely entered PreCalibrating from Idle";
      }
      disableOutput();
    }
  }
}

//---------------------------------------------------------------------------
// Pre- Calibrating Tests
//---------------------------------------------------------------------------

struct PreCalibratingTest : public StateTest {
  state_machine::PreCalibrating *state = state_machine::PreCalibrating::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 */
TEST_F(PreCalibratingTest, handlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    enableOutput();
    if (has_emergency) {
      ASSERT_EQ(new_state, state_machine::FailureStopped::getInstance())
        << "failed to enter FailureStopped from PreCalibrating";
    } else {
      ASSERT_NE(new_state, state_machine::FailureStopped::getInstance())
        << "falsely entered FailureStopped from PreCalibrating";
    }
    disableOutput();
  }
}

/**
 * Ensures that if no module reports an emergency and if
 * the calibrate command is received, the state changes
 * to the Calibrating state.
 */

TEST_F(PreCalibratingTest, handlesCalibrateCommand)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool calibrate_command = state_machine::checkCalibrateCommand(telemetry_data_);
      const auto new_state         = state->checkTransition(log_);

      enableOutput();
      if (calibrate_command) {
        ASSERT_EQ(new_state, state_machine::Calibrating::getInstance())
          << "failed to enter Calibrating from PreCalibrating";
      } else {
        ASSERT_NE(new_state, state_machine::Calibrating::getInstance())
          << "falsely entered Calibrating from PreCalibrating";
      }
      disableOutput();
    }
  }
}

//---------------------------------------------------------------------------
// Calibrating Tests
//---------------------------------------------------------------------------

/**
 * Testing Calibrating behaviour with respect to data
 */

struct CalibratingTest : public StateTest {
  state_machine::Calibrating *state = state_machine::Calibrating::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 */
TEST_F(CalibratingTest, handlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    enableOutput();
    if (has_emergency) {
      ASSERT_EQ(new_state, state_machine::FailureStopped::getInstance())
        << "failed to enter FailureStopped from Calibrating";
    } else {
      ASSERT_NE(new_state, state_machine::FailureStopped::getInstance())
        << "falsely entered FailureStopped from Calibrating";
    }
    disableOutput();
  }
}

/**
 * Ensures that if no module reports an emergency and if
 * all modules are ready after calibration, the state
 * changes to the ready state.
 */
TEST_F(CalibratingTest, handlesAllReady)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool all_ready
        = state_machine::checkModulesReady(log_, brakes_data_, nav_data_, batteries_data_,
                                           telemetry_data_, sensors_data_, motors_data_);
      const auto new_state = state->checkTransition(log_);

      enableOutput();
      if (all_ready) {
        ASSERT_EQ(new_state, state_machine::Ready::getInstance())
          << "failed to enter Ready from Calibrating";
      } else {
        ASSERT_NE(new_state, state_machine::Ready::getInstance())
          << "falsely entered Ready from Calibrating";
      }
      disableOutput();
    }
  }
}

//---------------------------------------------------------------------------
// Ready Tests
//---------------------------------------------------------------------------

/**
 * Testing Ready behaviour with respect to data
 */
struct ReadyTest : public StateTest {
  state_machine::Ready *state = state_machine::Ready::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(ReadyTest, handlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    enableOutput();
    if (has_emergency) {
      ASSERT_EQ(new_state, state_machine::FailureStopped::getInstance())
        << "failed to enter FailureStopped from Ready";
    } else {
      ASSERT_NE(new_state, state_machine::FailureStopped::getInstance())
        << "falsely entered FailureStopped from Ready";
    }
    disableOutput();
  }
}

/**
 * Ensures that if no module reports an emergency and if
 * the launch command is received while in the ready state,
 * the state changes to the accelerating state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(ReadyTest, handlesLaunchCommand)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool received_launch_command = state_machine::checkLaunchCommand(telemetry_data_);
      const auto new_state               = state->checkTransition(log_);

      enableOutput();
      if (received_launch_command) {
        ASSERT_EQ(new_state, state_machine::Accelerating::getInstance())
          << "failed to enter Accelerating from Ready";
      } else {
        ASSERT_NE(new_state, state_machine::Accelerating::getInstance())
          << "falsely entered Accelerating from Ready";
      }
      disableOutput();
    }
  }
}

//---------------------------------------------------------------------------
// Accelerating Tests
//---------------------------------------------------------------------------

/**
 * Testing Accelerating behaviour with respect to data
 */
struct AcceleratingTest : public StateTest {
  state_machine::Accelerating *state = state_machine::Accelerating::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureBraking.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(AcceleratingTest, handlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    enableOutput();
    if (has_emergency) {
      ASSERT_EQ(new_state, state_machine::FailureBraking::getInstance())
        << "failed to enter FailureBraking from Accelerating";
    } else {
      ASSERT_NE(new_state, state_machine::FailureBraking::getInstance())
        << "falsely entered FailureBraking from Accelerating";
    }
    disableOutput();
  }
}

/**
 * Ensures that if no emergency is reported from any module and
 * if the pod is in the braking zone, the state changes to the
 * nominal braking state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(AcceleratingTest, handlesInBrakingZone)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool in_braking_zone = state_machine::checkEnteredBrakingZone(log_, nav_data_);
      const auto new_state       = state->checkTransition(log_);

      enableOutput();
      if (in_braking_zone) {
        ASSERT_EQ(new_state, state_machine::NominalBraking::getInstance())
          << "failed to enter NominalBraking from Accelerating";
      } else {
        ASSERT_NE(new_state, state_machine::NominalBraking::getInstance())
          << "falsely entered NominalBraking from Accelerating";
      }
      disableOutput();
    }
  }
}

//---------------------------------------------------------------------------
// Nominal Braking Tests
//---------------------------------------------------------------------------

/**
 * Testing NominalBraking behaviour with respect to data
 */
struct NominalBrakingTest : public StateTest {
  state_machine::NominalBraking *state = state_machine::NominalBraking::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureBraking.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(NominalBrakingTest, handlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    enableOutput();
    if (has_emergency) {
      ASSERT_EQ(new_state, state_machine::FailureBraking::getInstance())
        << "failed to enter FailureBraking from NominalBraking";
    } else {
      ASSERT_NE(new_state, state_machine::FailureBraking::getInstance())
        << "falsely entered FailureBraking from NominalBraking";
    }
    disableOutput();
  }
}

/**
 * Ensures that if no emergency is reported from any module and
 * if the pod is stopped, the state changes to the finished state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(NominalBrakingTest, handlesStopped)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool stopped   = state_machine::checkPodStopped(log_, nav_data_);
      const auto new_state = state->checkTransition(log_);

      enableOutput();
      if (stopped) {
        ASSERT_EQ(new_state, state_machine::Finished::getInstance())
          << "failed to enter Finished from NominalBraking";
      } else {
        ASSERT_NE(new_state, state_machine::Finished::getInstance())
          << "falsely entered Finished from NominalBraking";
      }
      disableOutput();
    }
  }
}

//---------------------------------------------------------------------------
// Finished Tests
//---------------------------------------------------------------------------

/**
 * Testing Finished behaviour with respect to data
 */
struct FinishedTest : public StateTest {
  state_machine::Finished *state = state_machine::Finished::getInstance();
};

/**
 * Ensures that if the shutdown command is received while in the
 * finished state, the state changes to the off state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(FinishedTest, handlesShutdownCommand)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool received_shutdown_command = state_machine::checkShutdownCommand(telemetry_data_);
    const auto new_state                 = state->checkTransition(log_);

    enableOutput();
    if (received_shutdown_command) {
      ASSERT_EQ(new_state, state_machine::Off::getInstance())
        << "failed to enter Off from Finished";
    } else {
      ASSERT_NE(new_state, state_machine::Off::getInstance())
        << "falsely entered Off from Finished";
    }
    disableOutput();
  }
}

//---------------------------------------------------------------------------
// Failure Braking Tests
//---------------------------------------------------------------------------

/**
 * Testing failure Braking behaviour with respect to data
 */
struct FailureBrakingTest : public StateTest {
  state_machine::FailureBraking *state = state_machine::FailureBraking::getInstance();
};

/**
 * Ensures that if the pod is stopped while in the failure
 * braking state, the state changes to FailureStopped.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(FailureBrakingTest, handlesStopped)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool stopped   = state_machine::checkPodStopped(log_, nav_data_);
    const auto new_state = state->checkTransition(log_);

    enableOutput();
    if (stopped) {
      ASSERT_EQ(new_state, state_machine::FailureStopped::getInstance())
        << "failed to enter FailureStopped from FailureBraking";
    } else {
      ASSERT_NE(new_state, state_machine::FailureStopped::getInstance())
        << "falsely entered FailureStopped from FailureBraking";
    }
    disableOutput();
  }
}

//---------------------------------------------------------------------------
// Failure Stopped Tests
//---------------------------------------------------------------------------

/**
 * Testing FailureStopped behaviour with respect to data
 */
struct FailureStoppedTest : public StateTest {
  state_machine::FailureStopped *state = state_machine::FailureStopped::getInstance();
};

/**
 * Ensures that if the shutdown command is received while in the
 * failure stopped state, the state changes to the off state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(FailureStoppedTest, handlesShutdownCommand)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool received_shutdown_command = state_machine::checkShutdownCommand(telemetry_data_);
    const auto new_state                 = state->checkTransition(log_);

    enableOutput();
    if (received_shutdown_command) {
      ASSERT_EQ(new_state, state_machine::Off::getInstance())
        << "failed to enter Off from FailureStopped";
    } else {
      ASSERT_NE(new_state, state_machine::Off::getInstance())
        << "falsely entered Off from FailureStopped";
    }
    disableOutput();
  }
}

}  // namespace hyped::testing
