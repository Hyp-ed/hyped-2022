#include <fcntl.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include <data/data.hpp>
#include <random>
#include <state_machine/state.hpp>
#include <state_machine/transitions.hpp>
#include <string>
#include <utils/logger.hpp>
#include <vector>

#include "randomiser.hpp"
#include "test.hpp"

using namespace hyped::data;
using namespace hyped::state_machine;

//---------------------------------------------------------------------------
//--------------------------------- TESTS -----------------------------------
//---------------------------------------------------------------------------

/**
 * Struct used for testing state behaviour. Contains
 *
 * 1. Variables
 * 1. Logger
 * 2. Error messages
 * 3. Constant test size
 */

class StateTest : public hyped::testing::Test {
  Data &data_ = Data::getInstance();

 protected:
  EmergencyBrakes embrakes_data_;
  Navigation nav_data_;
  Batteries batteries_data_;
  Telemetry telemetry_data_;
  Sensors sensors_data_;
  Motors motors_data_;

  // ---- Test size -----------

  static constexpr int TEST_SIZE = 1000;

  // ---- Methods -------------

  /**
   * Generates random values for all the entries in data and overwrites them.
   */
  void randomiseData()
  {
    Randomiser::randomiseEmbrakes(embrakes_data_);
    Randomiser::randomiseNavigation(nav_data_);
    Randomiser::randomiseTelemetry(telemetry_data_);
    Randomiser::randomiseMotors(motors_data_);
    Randomiser::randomiseSensorsData(sensors_data_);
    Randomiser::randomiseBatteriesData(batteries_data_);

    data_.setEmergencyBrakesData(embrakes_data_);
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
  Idle *state = Idle::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 */
TEST_F(IdleTest, handlesEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool has_emergency = checkEmergency(log_, embrakes_data_, nav_data_, batteries_data_,
                                              telemetry_data_, sensors_data_, motors_data_);
    const auto new_state     = state->checkTransition(log_);

    enableOutput();
    if (has_emergency) {
      ASSERT_EQ(new_state, FailureStopped::getInstance())
        << "failed to enter FailureStopped from Idle";
    } else {
      ASSERT_NE(new_state, FailureStopped::getInstance())
        << "falsely entered FailureStopped from Idle";
    }
    disableOutput();
  }
}

/**
 * Ensures that if no module reports an emergency and if
 * the calibrate command is not received, a null pointer
 * is returned.
 */
TEST_F(IdleTest, handlesCalibrateCommand)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool has_emergency = checkEmergency(log_, embrakes_data_, nav_data_, batteries_data_,
                                              telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool calibrate_command = checkCalibrateCommand(log_, telemetry_data_);
      const auto new_state         = state->checkTransition(log_);

      if (!calibrate_command) {
        enableOutput();
        ASSERT_EQ(new_state, nullptr) << "falsely transitioned from Idle";
        disableOutput();
      }
    }
  }
}

/**
 * Ensures that if no module reports an emergency and if
 * the calibrate command is not received and if every module
 * is initialised, the state changes to the calibrating state.
 */
TEST_F(IdleTest, handlesAllInitialised)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool has_emergency = checkEmergency(log_, embrakes_data_, nav_data_, batteries_data_,
                                              telemetry_data_, sensors_data_, motors_data_);

    const bool calibrate_command = checkCalibrateCommand(log_, telemetry_data_);

    if (!has_emergency && calibrate_command) {
      const bool all_initialised
        = checkModulesInitialised(log_, embrakes_data_, nav_data_, batteries_data_, telemetry_data_,
                                  sensors_data_, motors_data_);
      const auto new_state = state->checkTransition(log_);

      enableOutput();
      if (all_initialised) {
        ASSERT_EQ(new_state, Calibrating::getInstance()) << "failed to enter Calibrating from Idle";
      } else {
        ASSERT_NE(new_state, Calibrating::getInstance()) << "falsely entered Calibrating from Idle";
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
  Calibrating *state = Calibrating::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 */
TEST_F(CalibratingTest, handlesEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool has_emergency = checkEmergency(log_, embrakes_data_, nav_data_, batteries_data_,
                                              telemetry_data_, sensors_data_, motors_data_);
    const auto new_state     = state->checkTransition(log_);

    enableOutput();
    if (has_emergency) {
      ASSERT_EQ(new_state, FailureStopped::getInstance())
        << "failed to enter FailureStopped from Calibrating";
    } else {
      ASSERT_NE(new_state, FailureStopped::getInstance())
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
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    bool has_emergency = checkEmergency(log_, embrakes_data_, nav_data_, batteries_data_,
                                        telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool all_ready = checkModulesReady(log_, embrakes_data_, nav_data_, batteries_data_,
                                               telemetry_data_, sensors_data_, motors_data_);
      const auto new_state = state->checkTransition(log_);

      enableOutput();
      if (all_ready) {
        ASSERT_EQ(new_state, Ready::getInstance()) << "failed to enter Ready from Calibrating";
      } else {
        ASSERT_NE(new_state, Ready::getInstance()) << "falsely entered Ready from Calibrating";
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
  Ready *state = Ready::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 *
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(ReadyTest, handlesEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool has_emergency = checkEmergency(log_, embrakes_data_, nav_data_, batteries_data_,
                                              telemetry_data_, sensors_data_, motors_data_);
    const auto new_state     = state->checkTransition(log_);

    enableOutput();
    if (has_emergency) {
      ASSERT_EQ(new_state, FailureStopped::getInstance())
        << "failed to enter FailureStopped from Ready";
    } else {
      ASSERT_NE(new_state, FailureStopped::getInstance())
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
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(ReadyTest, handlesLaunchCommand)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool has_emergency = checkEmergency(log_, embrakes_data_, nav_data_, batteries_data_,
                                              telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool received_launch_command = checkLaunchCommand(log_, telemetry_data_);
      const auto new_state               = state->checkTransition(log_);

      enableOutput();
      if (received_launch_command) {
        ASSERT_EQ(new_state, Accelerating::getInstance())
          << "failed to enter Accelerating from Ready";
      } else {
        ASSERT_NE(new_state, Accelerating::getInstance())
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
  Accelerating *state = Accelerating::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureBraking.
 *
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(AcceleratingTest, handlesEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool has_emergency = checkEmergency(log_, embrakes_data_, nav_data_, batteries_data_,
                                              telemetry_data_, sensors_data_, motors_data_);
    const auto new_state     = state->checkTransition(log_);

    enableOutput();
    if (has_emergency) {
      ASSERT_EQ(new_state, FailureBraking::getInstance())
        << "failed to enter FailureBraking from Accelerating";
    } else {
      ASSERT_NE(new_state, FailureBraking::getInstance())
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
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(AcceleratingTest, handlesInBrakingZone)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool has_emergency = checkEmergency(log_, embrakes_data_, nav_data_, batteries_data_,
                                              telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool in_braking_zone = checkEnteredBrakingZone(log_, nav_data_);
      const auto new_state       = state->checkTransition(log_);

      enableOutput();
      if (in_braking_zone) {
        ASSERT_EQ(new_state, NominalBraking::getInstance())
          << "failed to enter NominalBraking from Accelerating";
      } else {
        ASSERT_NE(new_state, NominalBraking::getInstance())
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
  NominalBraking *state = NominalBraking::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureBraking.
 *
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(NominalBrakingTest, handlesEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool has_emergency = checkEmergency(log_, embrakes_data_, nav_data_, batteries_data_,
                                              telemetry_data_, sensors_data_, motors_data_);
    const auto new_state     = state->checkTransition(log_);

    enableOutput();
    if (has_emergency) {
      ASSERT_EQ(new_state, FailureBraking::getInstance())
        << "failed to enter FailureBraking from NominalBraking";
    } else {
      ASSERT_NE(new_state, FailureBraking::getInstance())
        << "falsely entered FailureBraking from NominalBraking";
    }
    disableOutput();
  }
}

/**
 * Ensures that if no emergency is reported from any module and
 * if the pod is stopped, the state changes to the finished state.
 *
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(NominalBrakingTest, handlesStopped)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool has_emergency = checkEmergency(log_, embrakes_data_, nav_data_, batteries_data_,
                                              telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool stopped   = checkPodStopped(log_, nav_data_);
      const auto new_state = state->checkTransition(log_);

      enableOutput();
      if (stopped) {
        ASSERT_EQ(new_state, Finished::getInstance())
          << "failed to enter Finished from NominalBraking";
      } else {
        ASSERT_NE(new_state, Finished::getInstance())
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
  Finished *state = Finished::getInstance();
};

/**
 * Ensures that if the shutdown command is received while in the
 * finished state, the state changes to the off state.
 *
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(FinishedTest, handlesShutdownCommand)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool received_shutdown_command = checkShutdownCommand(log_, telemetry_data_);
    const auto new_state                 = state->checkTransition(log_);

    enableOutput();
    if (received_shutdown_command) {
      ASSERT_EQ(new_state, Off::getInstance()) << "failed to enter Off from Finished";
    } else {
      ASSERT_NE(new_state, Off::getInstance()) << "falsely entered Off from Finished";
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
  FailureBraking *state = FailureBraking::getInstance();
};

/**
 * Ensures that if the pod is stopped while in the failure
 * braking state, the state changes to FailureStopped.
 *
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(FailureBrakingTest, handlesStopped)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool stopped   = checkPodStopped(log_, nav_data_);
    const auto new_state = state->checkTransition(log_);

    enableOutput();
    if (stopped) {
      ASSERT_EQ(new_state, FailureStopped::getInstance())
        << "failed to enter FailureStopped from FailureBraking";
    } else {
      ASSERT_NE(new_state, FailureStopped::getInstance())
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
  FailureStopped *state = FailureStopped::getInstance();
};

/**
 * Ensures that if the shutdown command is received while in the
 * failure stopped state, the state changes to the off state.
 *
 * Time complexity: O(TEST_SIZE)
 */
TEST_F(FailureStoppedTest, handlesShutdownCommand)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    const bool received_shutdown_command = checkShutdownCommand(log_, telemetry_data_);
    const auto new_state                 = state->checkTransition(log_);

    enableOutput();
    if (received_shutdown_command) {
      ASSERT_EQ(new_state, Off::getInstance()) << "failed to enter Off from FailureStopped";
    } else {
      ASSERT_NE(new_state, Off::getInstance()) << "falsely entered Off from FailureStopped";
    }
    disableOutput();
  }
}
