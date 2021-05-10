/*
 * Author: Franz Miltz, Efe Ozbatur
 * Organisation: HYPED
 * Date:
 * Description:
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

#include <fcntl.h>
#include <stdlib.h>

#include <random>
#include <string>
#include <vector>

#include "data/data.hpp"
#include "gtest/gtest.h"
#include "randomiser.hpp"
#include "state_machine/state.hpp"
#include "state_machine/transitions.hpp"
#include "utils/logger.hpp"

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

struct StateTest : public ::testing::Test {
  // ---- Variables ------------

  Data &data = Data::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  // ---- Logger ---------------

  hyped::utils::Logger log;
  int stdout_f;
  int tmp_stdout_f;

  // ---- Error messages -------

  const std::string not_enter_emergency_error = "Does not enter emergency when required.";
  const std::string enter_emergency_error     = "Enters emergency when not required.";
  const std::string not_enter_off_error       = "Does not enter Off when required.";
  const std::string enter_off_error           = "Enters Off when not required.";
  const std::string not_enter_failure_stopped_error
    = "Does not enter Failure Stopped when required.";
  const std::string enter_failure_stopped_error = "Enters Failure Stopped when not required.";

  // ---- Test size -----------

  static constexpr int TEST_SIZE = 1000;

  // ---- Methods -------------

  /**
   * Generates random values for all the entries in data and overwrites them.
   */
  void randomiseData()
  {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);
    Randomiser::randomiseSensorsData(sensors_data);
    Randomiser::randomiseBatteriesData(batteries_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);
    data.setSensorsData(sensors_data);
    data.setBatteriesData(batteries_data);
  }

 protected:
  void SetUp()
  {
    fflush(stdout);
    stdout_f     = dup(1);
    tmp_stdout_f = open("/dev/null", O_WRONLY);
    dup2(tmp_stdout_f, 1);
    close(tmp_stdout_f);
  }

  void TearDown()
  {
    fflush(stdout);
    dup2(stdout_f, 1);
    close(stdout_f);
  }
};

//---------------------------------------------------------------------------
// Idle Tests
//---------------------------------------------------------------------------

/**
 * Testing Idle behaviour with respect to data
 */
struct IdleTest : public StateTest {
  Idle *state = Idle::getInstance();

  const std::string calibrate_command_error     = "Should handle calibrate command.";
  const std::string not_enter_calibrating_error = "Does not enter Calibrating when required.";
  const std::string enter_calibrating_error     = "Enters Calibrating when not required.";
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 */
TEST_F(IdleTest, handlesEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (has_emergency) {
      ASSERT_EQ(new_state, FailureStopped::getInstance()) << not_enter_emergency_error;
    } else {
      ASSERT_NE(new_state, FailureStopped::getInstance()) << enter_emergency_error;
    }
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

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);

    if (!has_emergency) {
      bool calibrate_command                 = checkCalibrateCommand(log, telemetry_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (!calibrate_command) { ASSERT_EQ(new_state, nullptr) << calibrate_command_error; }
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

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);

    bool calibrate_command = checkCalibrateCommand(log, telemetry_data);

    if (!has_emergency && calibrate_command) {
      bool all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                                     telemetry_data, sensors_data, motors_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (all_initialised) {
        ASSERT_EQ(new_state, Calibrating::getInstance()) << not_enter_calibrating_error;
      } else {
        ASSERT_NE(new_state, Calibrating::getInstance()) << enter_calibrating_error;
      }
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

  const std::string not_enter_ready_error = "Does not enter Ready when required.";
  const std::string enter_ready_error     = "Enters Ready when not required.";
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 */
TEST_F(CalibratingTest, handlesEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    randomiseData();

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (has_emergency) {
      ASSERT_EQ(new_state, FailureStopped::getInstance()) << not_enter_emergency_error;
    } else {
      ASSERT_NE(new_state, FailureStopped::getInstance()) << enter_emergency_error;
    }
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

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);

    if (!has_emergency) {
      bool all_ready = checkModulesReady(log, embrakes_data, nav_data, motors_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (all_ready) {
        ASSERT_EQ(new_state, Ready::getInstance()) << not_enter_ready_error;
      } else {
        ASSERT_NE(new_state, Ready::getInstance()) << enter_ready_error;
      }
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

  const std::string not_enter_accelerating_error = "Does not enter Accelerating when required.";
  const std::string enter_accelerating_error     = "Enters Accelerating when not required.";
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

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (has_emergency) {
      ASSERT_EQ(new_state, FailureStopped::getInstance()) << not_enter_emergency_error;
    } else {
      ASSERT_NE(new_state, FailureStopped::getInstance()) << enter_emergency_error;
    }
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

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);

    if (!has_emergency) {
      bool received_launch_command           = checkLaunchCommand(log, telemetry_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (received_launch_command) {
        ASSERT_EQ(new_state, Accelerating::getInstance()) << not_enter_accelerating_error;
      } else {
        ASSERT_NE(new_state, Accelerating::getInstance()) << enter_accelerating_error;
      }
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

  const std::string not_enter_braking_error = "Does not enter Nominal Braking when required.";
  const std::string enter_braking_error     = "Enters Nominal Braking when not required.";
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

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (has_emergency) {
      ASSERT_EQ(new_state, FailureBraking::getInstance()) << not_enter_emergency_error;
    } else {
      ASSERT_NE(new_state, FailureBraking::getInstance()) << enter_emergency_error;
    }
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

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);

    if (!has_emergency) {
      bool in_braking_zone                   = checkEnteredBrakingZone(log, nav_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (in_braking_zone) {
        ASSERT_EQ(new_state, NominalBraking::getInstance()) << not_enter_braking_error;
      } else {
        ASSERT_NE(new_state, NominalBraking::getInstance()) << enter_braking_error;
      }
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

  const std::string not_enter_finished_error = "Does not enter Finished when required.";
  const std::string enter_finished_error     = "Enters Finished when not required.";
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

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (has_emergency) {
      ASSERT_EQ(new_state, FailureBraking::getInstance()) << not_enter_emergency_error;
    } else {
      ASSERT_NE(new_state, FailureBraking::getInstance()) << enter_emergency_error;
    }
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

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);

    if (!has_emergency) {
      bool stopped                           = checkPodStopped(log, nav_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (stopped) {
        ASSERT_EQ(new_state, Finished::getInstance()) << not_enter_finished_error;
      } else {
        ASSERT_NE(new_state, Finished::getInstance()) << enter_finished_error;
      }
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

    bool received_shutdown_command         = checkShutdownCommand(log, telemetry_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (received_shutdown_command) {
      ASSERT_EQ(new_state, Off::getInstance()) << not_enter_off_error;
    } else {
      ASSERT_NE(new_state, Off::getInstance()) << enter_off_error;
    }
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

    bool stopped                           = checkPodStopped(log, nav_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (stopped) {
      ASSERT_EQ(new_state, FailureStopped::getInstance()) << not_enter_failure_stopped_error;
    } else {
      ASSERT_NE(new_state, FailureStopped::getInstance()) << enter_failure_stopped_error;
    }
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

    bool received_shutdown_command         = checkShutdownCommand(log, telemetry_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (received_shutdown_command) {
      ASSERT_EQ(new_state, Off::getInstance()) << not_enter_off_error;
    } else {
      ASSERT_NE(new_state, Off::getInstance()) << enter_off_error;
    }
  }
}
