#include "demorandomiser.hpp"
#include "test.hpp"

#include <fcntl.h>
#include <stdlib.h>

#include <chrono>
#include <random>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <data/data.hpp>
#include <demo_state_machine/state.hpp>
#include <demo_state_machine/transitions.hpp>
#include <utils/concurrent/thread.hpp>
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
  data::StateMachine stm_data_;

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
};

//---------------------------------------------------------------------------
// Idle Tests
//---------------------------------------------------------------------------

/**
 * Testing Idle behaviour with respect to data
 */
class DemoIdleTest : public StateTest {
 protected:
  demo_state_machine::Idle *state = demo_state_machine::Idle::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 */
TEST_F(DemoIdleTest, demoHandlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    if (has_emergency) {
      ASSERT_EQ(new_state, demo_state_machine::FailureStopped::getInstance())
        << "failed to enter FailureStopped from Idle";
    } else {
      ASSERT_NE(new_state, demo_state_machine::FailureStopped::getInstance())
        << "falsely entered FailureStopped from Idle";
    }
  }
}

/**
 * Ensures that if no module reports an emergency and if every module
 * is initialised, the state changes to the PreCalibrating state.
 */
TEST_F(DemoIdleTest, demoHandlesAllInitialised)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool all_initialised = demo_state_machine::checkModulesInitialised(
        log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_,
        motors_data_);
      const auto new_state = state->checkTransition(log_);

      if (all_initialised) {
        ASSERT_EQ(new_state, demo_state_machine::PreCalibrating::getInstance())
          << "failed to enter PreCalibrating from Idle";
      } else {
        ASSERT_NE(new_state, demo_state_machine::PreCalibrating::getInstance())
          << "falsely entered PreCalibrating from Idle";
      }
    }
  }
}

//---------------------------------------------------------------------------
// Pre- Calibrating Tests
//---------------------------------------------------------------------------

class DemoPreCalibratingTest : public StateTest {
 protected:
  demo_state_machine::PreCalibrating *state = demo_state_machine::PreCalibrating::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 */
TEST_F(DemoPreCalibratingTest, demoHandlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    if (has_emergency) {
      ASSERT_EQ(new_state, demo_state_machine::FailureStopped::getInstance())
        << "failed to enter FailureStopped from PreCalibrating";
    } else {
      ASSERT_NE(new_state, demo_state_machine::FailureStopped::getInstance())
        << "falsely entered FailureStopped from PreCalibrating";
    }
  }
}

/**
 * Ensures that if no module reports an emergency and if
 * the calibrate command is received, the state changes
 * to the Calibrating state.
 */

TEST_F(DemoPreCalibratingTest, demoHandlesCalibrateCommand)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool calibrate_command = demo_state_machine::checkCalibrateCommand(telemetry_data_);
      const auto new_state         = state->checkTransition(log_);

      if (calibrate_command) {
        ASSERT_EQ(new_state, demo_state_machine::Calibrating::getInstance())
          << "failed to enter Calibrating from PreCalibrating";
      } else {
        ASSERT_NE(new_state, demo_state_machine::Calibrating::getInstance())
          << "falsely entered Calibrating from PreCalibrating";
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

class DemoCalibratingTest : public StateTest {
 protected:
  demo_state_machine::Calibrating *state = demo_state_machine::Calibrating::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 */
TEST_F(DemoCalibratingTest, demoHandlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    if (has_emergency) {
      ASSERT_EQ(new_state, demo_state_machine::FailureStopped::getInstance())
        << "failed to enter FailureStopped from Calibrating";
    } else {
      ASSERT_NE(new_state, demo_state_machine::FailureStopped::getInstance())
        << "falsely entered FailureStopped from Calibrating";
    }
  }
}

/**
 * Ensures that if no module reports an emergency and if
 * all modules are ready after calibration, the state
 * changes to the pre-ready state.
 */
TEST_F(DemoCalibratingTest, demoHandlesAllReady)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool all_ready
        = demo_state_machine::checkModulesReady(log_, brakes_data_, nav_data_, batteries_data_,
                                                telemetry_data_, sensors_data_, motors_data_);
      const auto new_state = state->checkTransition(log_);

      if (all_ready) {
        ASSERT_EQ(new_state, demo_state_machine::PreReady::getInstance())
          << "failed to enter PreReady from Calibrating";
      } else {
        ASSERT_NE(new_state, demo_state_machine::PreReady::getInstance())
          << "falsely entered PreReady from Calibrating";
      }
    }
  }
}

//---------------------------------------------------------------------------
// PreReady Tests
//---------------------------------------------------------------------------

/**
 * Testing PreReady behaviour with respect to data
 */
class DemoPreReadyTest : public StateTest {
 protected:
  demo_state_machine::PreReady *state = demo_state_machine::PreReady::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoPreReadyTest, demoHandlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    if (has_emergency) {
      ASSERT_EQ(new_state, demo_state_machine::FailureStopped::getInstance())
        << "failed to enter FailureStopped from PreReady";
    } else {
      ASSERT_NE(new_state, demo_state_machine::FailureStopped::getInstance())
        << "falsely entered FailureStopped from PreReady";
    }
  }
}

/**
 * Ensures that if no module reports an emergency and if
 * all SSRs are in HP then state changes to the Ready state
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoPreReadyTest, demoHandlesHighPowerOn)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool has_high_power_on = !demo_state_machine::checkHighPowerOff(sensors_data_);
      const auto new_state         = state->checkTransition(log_);

      if (has_high_power_on) {
        ASSERT_EQ(new_state, demo_state_machine::Ready::getInstance())
          << "failed to enter Ready from PreReady";
      } else {
        ASSERT_NE(new_state, demo_state_machine::Ready::getInstance())
          << "falsely entered Ready from PreReady";
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
class DemoReadyTest : public StateTest {
 protected:
  demo_state_machine::Ready *state = demo_state_machine::Ready::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureStopped.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoReadyTest, demoHandlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    if (has_emergency) {
      ASSERT_EQ(new_state, demo_state_machine::FailureStopped::getInstance())
        << "failed to enter FailureStopped from Ready";
    } else {
      ASSERT_NE(new_state, demo_state_machine::FailureStopped::getInstance())
        << "falsely entered FailureStopped from Ready";
    }
  }
}

/**
 * Ensures that if no module reports an emergency and if
 * the launch command is received while in the ready state,
 * the state changes to the accelerating state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoReadyTest, demoHandlesLaunchCommand)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool received_launch_command = demo_state_machine::checkLaunchCommand(telemetry_data_);
      const auto new_state               = state->checkTransition(log_);

      if (received_launch_command) {
        ASSERT_EQ(new_state, demo_state_machine::Accelerating::getInstance())
          << "failed to enter Accelerating from Ready";
      } else {
        ASSERT_NE(new_state, demo_state_machine::Accelerating::getInstance())
          << "falsely entered Accelerating from Ready";
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
class DemoAcceleratingTest : public StateTest {
 protected:
  demo_state_machine::Accelerating *state = demo_state_machine::Accelerating::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailurePreBraking.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoAcceleratingTest, demoHandlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    if (has_emergency) {
      ASSERT_EQ(new_state, demo_state_machine::FailurePreBraking::getInstance())
        << "failed to enter FailureBraking from Accelerating";
    } else {
      ASSERT_NE(new_state, demo_state_machine::FailurePreBraking::getInstance())
        << "falsely entered FailureBraking from Accelerating";
    }
  }
}

/**
 * Ensures that if no emergency is reported from any module and
 * if the pod is in the braking zone, the state changes to the
 * pre-braking state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoAcceleratingTest, handlesInBrakingZone)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool in_braking_zone = demo_state_machine::checkEnteredBrakingZone(log_, nav_data_);
      const auto new_state       = state->checkTransition(log_);

      if (in_braking_zone) {
        ASSERT_EQ(new_state, demo_state_machine::PreBraking::getInstance())
          << "failed to enter PreBraking from Accelerating";
      } else {
        ASSERT_NE(new_state, demo_state_machine::PreBraking::getInstance())
          << "falsely entered PreBraking from Accelerating";
      }
    }
  }
}

/**
 * Ensures that if no emergency is reported from any module and
 * if the pod has reached maximum velocity, the state changes to the
 * Cruising state.
 *
 * Time complexity: O(kTestSize)
 */

TEST_F(DemoAcceleratingTest, demoHandlesAcceleratingTimePassed)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    // Assert not in braking zone
    nav_data_.braking_distance = 0;
    nav_data_.displacement     = 0;

    // Asserting sufficient time has passed
    utils::concurrent::Thread::sleep(400);  // 0.4s

    // reading and writing to the CDS directly to update navigation data
    auto &data_ = data::Data::getInstance();
    data_.setNavigationData(nav_data_);

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool has_acceleration_time_exceeded
        = demo_state_machine::checkAccelerationTimeExceeded(stm_data_);
      const auto new_state = state->checkTransition(log_);

      if (has_acceleration_time_exceeded) {
        ASSERT_EQ(new_state, demo_state_machine::Cruising::getInstance())
          << "failed to enter Cruising from Accelerating";
      } else {
        ASSERT_NE(new_state, demo_state_machine::Cruising::getInstance())
          << "falsely entered Cruising from Accelerating";
      }
    }
  }
}

//---------------------------------------------------------------------------
// Cruising Tests
//---------------------------------------------------------------------------

/**
 * Testing Cruising behaviour with respect to data
 */
class DemoCruisingTest : public StateTest {
 protected:
  demo_state_machine::Cruising *state = demo_state_machine::Cruising::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailurePreBraking.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoCruisingTest, demoHandlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    if (has_emergency) {
      ASSERT_EQ(new_state, demo_state_machine::FailurePreBraking::getInstance())
        << "failed to enter FailurePreBraking from Cruising";
    } else {
      ASSERT_NE(new_state, demo_state_machine::FailurePreBraking::getInstance())
        << "falsely entered FailurePreBraking from Cruising";
    }
  }
}

/**
 * Ensures that if no emergency is reported from any module and
 * if the pod is in the braking zone, the state changes to the
 * pre-braking state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoCruisingTest, demoHandlesInBrakingZone)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool in_braking_zone = demo_state_machine::checkEnteredBrakingZone(log_, nav_data_);
      const auto new_state       = state->checkTransition(log_);

      if (in_braking_zone) {
        ASSERT_EQ(new_state, demo_state_machine::PreBraking::getInstance())
          << "failed to enter PreBraking from Cruising";
      } else {
        ASSERT_NE(new_state, demo_state_machine::PreBraking::getInstance())
          << "falsely entered PreBraking from Cruising";
      }
    }
  }
}
//---------------------------------------------------------------------------
// Pre-Braking Tests
//---------------------------------------------------------------------------

/**
 * Testing PreBraking behaviour with respect to data
 */
class DemoPreBrakingTest : public StateTest {
 protected:
  demo_state_machine::PreBraking *state = demo_state_machine::PreBraking::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailurePreBraking.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoPreBrakingTest, demoHandlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    if (has_emergency) {
      ASSERT_EQ(new_state, demo_state_machine::FailurePreBraking::getInstance())
        << "failed to enter FailurePreBraking from PreBraking";
    } else {
      ASSERT_NE(new_state, demo_state_machine::FailurePreBraking::getInstance())
        << "falsely entered FailurePreBraking from PreBraking";
    }
  }
}

/**
 * Ensures that if no emergency is reported from any module and
 * if all the SSRs are not in HP, the state changes to the nominal braking state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoPreBrakingTest, demoHandlesHighPowerOff)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool has_high_power_off = demo_state_machine::checkHighPowerOff(sensors_data_);
      const auto new_state          = state->checkTransition(log_);

      if (has_high_power_off) {
        ASSERT_EQ(new_state, demo_state_machine::NominalBraking::getInstance())
          << "failed to enter NominalBraking from PreBraking";
      } else {
        ASSERT_NE(new_state, demo_state_machine::NominalBraking::getInstance())
          << "falsely entered NominalBraking from PreBraking";
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
class DemoNominalBrakingTest : public StateTest {
 protected:
  demo_state_machine::NominalBraking *state = demo_state_machine::NominalBraking::getInstance();
};

/**
 * Ensures that if any module reports an emergency,
 * the state changes to FailureBraking.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoNominalBrakingTest, demoHandlesEmergency)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const auto new_state = state->checkTransition(log_);

    if (has_emergency) {
      ASSERT_EQ(new_state, demo_state_machine::FailureBraking::getInstance())
        << "failed to enter FailureBraking from NominalBraking";
    } else {
      ASSERT_NE(new_state, demo_state_machine::FailureBraking::getInstance())
        << "falsely entered FailureBraking from NominalBraking";
    }
  }
}

/**
 * Ensures that if no emergency is reported from any module and
 * if the pod is stopped, the state changes to the finished state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoNominalBrakingTest, demoHandlesStopped)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    if (!has_emergency) {
      const bool stopped   = demo_state_machine::checkPodStopped(log_, nav_data_);
      const auto new_state = state->checkTransition(log_);

      if (stopped) {
        ASSERT_EQ(new_state, demo_state_machine::Finished::getInstance())
          << "failed to enter Finished from NominalBraking";
      } else {
        ASSERT_NE(new_state, demo_state_machine::Finished::getInstance())
          << "falsely entered Finished from NominalBraking";
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
class DemoFinishedTest : public StateTest {
 protected:
  demo_state_machine::Finished *state = demo_state_machine::Finished::getInstance();
};

/**
 * Ensures that if the shutdown command is received while in the
 * finished state, the state changes to the off state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoFinishedTest, demoHandlesShutdownCommand)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool received_shutdown_command
      = demo_state_machine::checkShutdownCommand(telemetry_data_);
    const auto new_state = state->checkTransition(log_);

    if (received_shutdown_command) {
      ASSERT_EQ(new_state, demo_state_machine::Off::getInstance())
        << "failed to enter Off from Finished";
    } else {
      ASSERT_NE(new_state, demo_state_machine::Off::getInstance())
        << "falsely entered Off from Finished";
    }
  }
}

//---------------------------------------------------------------------------
// Failure Pre-Braking Tests
//---------------------------------------------------------------------------

/**
 * Testing failure pre-braking behaviour with respect to data
 */
class DemoFailurePreBrakingTest : public StateTest {
 protected:
  demo_state_machine::FailurePreBraking *state
    = demo_state_machine::FailurePreBraking::getInstance();
};

/**
 * Ensures that if SSRs are not in HP while in the failure
 * pre-braking state, the state changes to FailureBraking.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoFailurePreBrakingTest, demoHandlesHighPowerOff)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool has_high_power_off = demo_state_machine::checkHighPowerOff(sensors_data_);
    const auto new_state          = state->checkTransition(log_);

    if (has_high_power_off) {
      ASSERT_EQ(new_state, demo_state_machine::FailureBraking::getInstance())
        << "failed to enter FailureBraking from FailurePreBraking";
    } else {
      ASSERT_NE(new_state, demo_state_machine::FailureBraking::getInstance())
        << "falsely entered FailureBraking from FailurePreBraking";
    }
  }
}

//---------------------------------------------------------------------------
// Failure Braking Tests
//---------------------------------------------------------------------------

/**
 * Testing failure Braking behaviour with respect to data
 */
class DemoFailureBrakingTest : public StateTest {
 protected:
  demo_state_machine::FailureBraking *state = demo_state_machine::FailureBraking::getInstance();
};

/**
 * Ensures that if the pod is stopped while in the failure
 * braking state, the state changes to FailureStopped.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoFailureBrakingTest, demoHandlesStopped)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool stopped   = demo_state_machine::checkPodStopped(log_, nav_data_);
    const auto new_state = state->checkTransition(log_);

    if (stopped) {
      ASSERT_EQ(new_state, demo_state_machine::FailureStopped::getInstance())
        << "failed to enter FailureStopped from FailureBraking";
    } else {
      ASSERT_NE(new_state, demo_state_machine::FailureStopped::getInstance())
        << "falsely entered FailureStopped from FailureBraking";
    }
  }
}

//---------------------------------------------------------------------------
// Failure Stopped Tests
//---------------------------------------------------------------------------

/**
 * Testing FailureStopped behaviour with respect to data
 */
class DemoFailureStoppedTest : public StateTest {
 protected:
  demo_state_machine::FailureStopped *state = demo_state_machine::FailureStopped::getInstance();
};

/**
 * Ensures that if the shutdown command is received while in the
 * failure stopped state, the state changes to the off state.
 *
 * Time complexity: O(kTestSize)
 */
TEST_F(DemoFailureStoppedTest, demoHandlesShutdownCommand)
{
  for (int i = 0; i < kTestSize; i++) {
    randomiseData();

    const bool received_shutdown_command
      = demo_state_machine::checkShutdownCommand(telemetry_data_);
    const auto new_state = state->checkTransition(log_);

    if (received_shutdown_command) {
      ASSERT_EQ(new_state, demo_state_machine::Off::getInstance())
        << "failed to enter Off from FailureStopped";
    } else {
      ASSERT_NE(new_state, demo_state_machine::Off::getInstance())
        << "falsely entered Off from FailureStopped";
    }
  }
}

}  // namespace hyped::testing
