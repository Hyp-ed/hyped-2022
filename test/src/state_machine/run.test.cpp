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
#include "state_machine/main.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"

using namespace hyped::data;
using namespace hyped::state_machine;
using hyped::utils::System;
using hyped::utils::concurrent::Thread;

struct RunTest : public ::testing::Test {
  static constexpr int TEST_SIZE = 10;

  // ---- Logger ---------------

  // Logger that is used by the test thread.
  hyped::utils::Logger log;

  // Logger that is used by the state machine thread during testing.
  hyped::utils::Logger stm_log;

  // File descriptors for the original and the temporary standard output stream to avoid logging
  // during testing.
  int stdout_f;
  int tmp_stdout_f;

  // ---- Error messages -------

  // Messages for critical failure encounters:

  const std::string idle_failure_error        = "Encountered critical failure during Idle.";
  const std::string calibrating_failure_error = "Encountered critical failure during Calibrating.";
  const std::string ready_failure_error       = "Encountered critical failure during Ready.";
  const std::string accelerating_failure_error
    = "Encountered critical failure during Accelerating.";
  const std::string nominal_braking_failure_error
    = "Encountered critical failure during NominalBraking.";

  // Messages for transition failures:

  const std::string idle_calibrating_transition_error
    = "Did not transition from Idle to Calibrating.";
  const std::string idle_failure_transition_error
    = "Did not transition from Idle to FailureStopped.";
  const std::string calibrating_ready_transition_erorr
    = "Did not transition from Calibrating to Ready.";
  const std::string calibrating_failure_transition_error
    = "Did not transition from Calibrating to Failure Stopped.";
  const std::string ready_accelerating_transition_error
    = "Did not transition from Ready to Accelerating.";
  const std::string ready_failure_transition_error
    = "Did not transition from Ready to Failure Stopped.";
  const std::string accelerating_braking_transition_error
    = "Did not transition from Accelerating to Nominal Braking.";
  const std::string accelerating_cruising_transition_error
    = "Did not transition from Accelerating to Cruising.";
  const std::string accelerating_failure_transition_error
    = "Did not transition from Accelerating to Failure Braking.";
  const std::string cruising_braking_transition_error
    = "Did not transition from Cruising to Nominal Braking.";
  const std::string cruising_failure_transition_error
    = "Did not transition from Cruising to Failure Braking.";
  const std::string braking_finished_transition_error
    = "Did not transition from Nominal Braking to Finished.";
  const std::string braking_failure_transition_error
    = "Did not transition from Nominal Braking to Failure Braking.";
  const std::string finished_off_transition_error = "Did not transition from Finished to Off.";
  const std::string failure_braking_stopped_transition_error
    = "Did not transition from Failure Braking to Failure Stopped.";
  const std::string failure_stopped_off_transition_error
    = "Did not transition from Failure Stopped to Off.";

  // ---- Data -----------------

  Data &data = Data::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;
  StateMachine stm_data;

  // ---- Utilities -------------

  bool output_enabled = true;

  /**
   * Reroutes stdout into /dev/null and saves the original output stream to be restored later.
   */
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

  /**
   *  Restores the original output stream to stdout.
   */
  void enableOutput()
  {
    if (output_enabled) { return; }
    output_enabled = true;
    fflush(stdout);
    dup2(stdout_f, 1);
    close(stdout_f);
  }

  /**
   *  The whole data is randomised before updating the necessary data.
   */
  void randomiseInternally()
  {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);
    Randomiser::randomiseSensorsData(sensors_data);
    Randomiser::randomiseBatteriesData(batteries_data);
  }

  /**
   *  This method is used to force the emergency case.
   */
  void forceEmergency()
  {
    // We only need to set one critical failure as other behaviour
    // is tested in transitions.test.cpp.
    embrakes_data.module_status = ModuleStatus::kCriticalFailure;
  }

  /**
   *  The updated data is written.
   */
  void writeData()
  {
    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);
    data.setSensorsData(sensors_data);
    data.setBatteriesData(batteries_data);
  }

  /**
   *  The written data is read.
   */
  void readData()
  {
    embrakes_data  = data.getEmergencyBrakesData();
    stm_data       = data.getStateMachineData();
    nav_data       = data.getNavigationData();
    telemetry_data = data.getTelemetryData();
    motors_data    = data.getMotorData();
    sensors_data   = data.getSensorsData();
    batteries_data = data.getBatteriesData();
  }

  /**
   * Allows the state machine thread to process the central data structure and transition between
   * states.
   *
   * If tests are failing for no apparent reason, try increasing the sleep duration to be sure this
   * is not the source of the problem.
   */
  void waitForUpdate() { Thread::sleep(10); }

  // ---- Run steps --------------

  /**
   * Simulating program start up and making sure undesired transitions from Idle are being
   * prevented.
   */
  void initialiseData()
  {
    readData();

    // Randomise data
    randomiseInternally();

    // Prevent Idle -> FailureStopped
    telemetry_data.emergency_stop_command = false;

    // Prevent Idle -> Calibrating
    embrakes_data.module_status      = ModuleStatus::kStart;
    nav_data.module_status           = ModuleStatus::kStart;
    telemetry_data.module_status     = ModuleStatus::kStart;
    motors_data.module_status        = ModuleStatus::kStart;
    sensors_data.module_status       = ModuleStatus::kStart;
    batteries_data.module_status     = ModuleStatus::kStart;
    telemetry_data.calibrate_command = false;

    // Verify transition conditions are as intended
    bool has_emergency           = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_calibrate_command   = checkCalibrateCommand(log, telemetry_data);
    bool has_modules_initialised = checkModulesInitialised(
      log, embrakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);

    enableOutput();
    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(false, has_calibrate_command);
    ASSERT_EQ(false, has_modules_initialised);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();
  }

  /**
   * Modifies data such that the Idle -> Calibrating transition conditions are met and verifies the
   * behaviour.
   */
  void testIdleToCalibrating()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kIdle);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Prevent Idle -> FailureStopped
    telemetry_data.emergency_stop_command = false;

    // Enforce Idle -> Calibrating
    telemetry_data.calibrate_command = true;
    embrakes_data.module_status      = ModuleStatus::kInit;
    nav_data.module_status           = ModuleStatus::kInit;
    telemetry_data.module_status     = ModuleStatus::kInit;
    motors_data.module_status        = ModuleStatus::kInit;
    sensors_data.module_status       = ModuleStatus::kInit;
    batteries_data.module_status     = ModuleStatus::kInit;

    // Prevent Calibrating -> Ready
    // >> No work required due to the above

    // Verify transition conditions are as intended
    bool has_emergency           = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_calibrating_command = checkCalibrateCommand(log, telemetry_data);
    bool has_modules_initialised = checkModulesInitialised(
      log, embrakes_data, nav_data, batteries_data, telemetry_data, sensors_data, motors_data);
    bool has_modules_ready = checkModulesReady(log, embrakes_data, nav_data, batteries_data,
                                               telemetry_data, sensors_data, motors_data);

    enableOutput();
    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_calibrating_command);
    ASSERT_EQ(true, has_modules_initialised);
    ASSERT_EQ(false, has_modules_ready);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << idle_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kCalibrating)
      << idle_calibrating_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the Idle -> FailureStopped transition conditions are met and verifies
   * the behaviour.
   */
  void testIdleEmergency()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kIdle);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Enforce Idle -> FailureStopped
    forceEmergency();

    // Prevent FailureStopped -> Off
    telemetry_data.shutdown_command = false;

    // Verify transition conditions are as intended
    bool has_emergency        = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_shutdown_command = checkShutdownCommand(log, telemetry_data);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_shutdown_command);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << idle_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFailureStopped)
      << idle_failure_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the Calibrating -> Ready transition conditions are met and verifies
   * the behaviour.
   */
  void testCalibratingToReady()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kCalibrating);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Prevent Calibrating -> FailureStopped
    telemetry_data.emergency_stop_command = false;

    // Enforce Calibrating -> Ready
    embrakes_data.module_status  = ModuleStatus::kReady;
    nav_data.module_status       = ModuleStatus::kReady;
    telemetry_data.module_status = ModuleStatus::kReady;
    motors_data.module_status    = ModuleStatus::kReady;
    sensors_data.module_status   = ModuleStatus::kReady;
    batteries_data.module_status = ModuleStatus::kReady;

    // Prevent Ready -> Accelerating
    telemetry_data.launch_command = false;

    // Verify transition conditions are as intended
    bool has_emergency      = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_modules_ready  = checkModulesReady(log, embrakes_data, nav_data, batteries_data,
                                               telemetry_data, sensors_data, motors_data);
    bool has_launch_command = checkLaunchCommand(log, telemetry_data);

    enableOutput();
    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_modules_ready);
    ASSERT_EQ(false, has_launch_command);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << calibrating_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kReady)
      << calibrating_ready_transition_erorr;
    disableOutput();
  }

  /**
   * Modifies data such that the Calibrating -> FailureStopped transition conditions are met and
   * verifies the behaviour.
   */
  void testCalibratingEmergency()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kCalibrating);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Enforce Calibrating -> FailureStopped
    forceEmergency();

    // Prevent FailureStopped -> Off
    telemetry_data.shutdown_command = false;

    // Verify transition conditions are as intended
    bool has_emergency        = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_shutdown_command = checkShutdownCommand(log, telemetry_data);

    enableOutput();
    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_shutdown_command);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << calibrating_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFailureStopped)
      << calibrating_failure_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the Ready -> Accelerating transition conditions are met and
   * verifies the behaviour.
   */
  void testReadyToAccelerating()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kReady);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Prevent Ready -> FailureStopped
    embrakes_data.module_status           = ModuleStatus::kReady;
    nav_data.module_status                = ModuleStatus::kReady;
    telemetry_data.module_status          = ModuleStatus::kReady;
    motors_data.module_status             = ModuleStatus::kReady;
    sensors_data.module_status            = ModuleStatus::kReady;
    batteries_data.module_status          = ModuleStatus::kReady;
    telemetry_data.emergency_stop_command = false;

    // Enforce Ready -> Accelerating
    telemetry_data.launch_command = true;

    // Prevent Accelerating -> NominalBraking
    nav_data.displacement     = 0;
    nav_data.braking_distance = 0;

    // Prevent Accelerating -> Cruising
    nav_data.velocity = Navigation::kMaximumVelocity / 2;

    // Verify transition conditions are as intended
    bool has_emergency            = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_launch_command       = checkLaunchCommand(log, telemetry_data);
    bool has_entered_braking_zone = checkEnteredBrakingZone(log, nav_data);
    bool has_reached_max_velocity = checkReachedMaxVelocity(log, nav_data);

    enableOutput();
    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_launch_command);
    ASSERT_EQ(false, has_entered_braking_zone);
    ASSERT_EQ(false, has_reached_max_velocity);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << ready_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kAccelerating)
      << ready_accelerating_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the Ready -> FailureStopped transition conditions are met and
   * verifies the behaviour.
   */
  void testReadyEmergency()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kReady);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Enforce Ready -> FailureStopped
    forceEmergency();

    // Prevent FailureStopped -> Off
    telemetry_data.shutdown_command = false;

    // Verify transition conditions are as intended
    bool has_emergency        = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_shutdown_command = checkShutdownCommand(log, telemetry_data);

    enableOutput();
    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_shutdown_command);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << ready_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFailureStopped)
      << ready_failure_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the Accelerating -> NominalBraking transition conditions are met and
   * verifies the behaviour.
   */
  void testAcceleratingToNominalBraking()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kAccelerating);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Prevent Accelerating -> FailureBraking
    embrakes_data.module_status           = ModuleStatus::kReady;
    nav_data.module_status                = ModuleStatus::kReady;
    telemetry_data.module_status          = ModuleStatus::kReady;
    motors_data.module_status             = ModuleStatus::kReady;
    sensors_data.module_status            = ModuleStatus::kReady;
    batteries_data.module_status          = ModuleStatus::kReady;
    telemetry_data.emergency_stop_command = false;

    // Enforce Accelerating -> NominalBraking
    nav_data.braking_distance = 1000;
    nav_data.displacement     = Navigation::kRunLength - nav_data.braking_distance;

    // Prevent NominalBraking -> Finished
    nav_data.velocity = 100;

    // Verify transition conditions are as intended
    bool has_emergency            = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_entered_braking_zone = checkEnteredBrakingZone(log, nav_data);
    bool has_stopped              = checkPodStopped(log, nav_data);

    enableOutput();
    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_entered_braking_zone);
    ASSERT_EQ(false, has_stopped);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << accelerating_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kNominalBraking)
      << accelerating_braking_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the Accelerating -> Cruising transition conditions are met and
   * verifies the behaviour.
   */
  void testAcceleratingToCruising()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kAccelerating);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Prevent Accelerating -> FailureBraking
    embrakes_data.module_status           = ModuleStatus::kReady;
    nav_data.module_status                = ModuleStatus::kReady;
    telemetry_data.module_status          = ModuleStatus::kReady;
    motors_data.module_status             = ModuleStatus::kReady;
    sensors_data.module_status            = ModuleStatus::kReady;
    batteries_data.module_status          = ModuleStatus::kReady;
    telemetry_data.emergency_stop_command = false;

    // Prevent Accelerating -> NominalBraking
    // Prevent Cruising -> NominalBraking
    nav_data.braking_distance = 0;
    nav_data.displacement     = 0;

    // Enforce Accelerating -> Cruising
    nav_data.velocity = Navigation::kMaximumVelocity;

    // Verify transition conditions are as intended
    bool has_emergency            = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_entered_braking_zone = checkEnteredBrakingZone(log, nav_data);
    bool has_reached_max_velocity = checkReachedMaxVelocity(log, nav_data);

    enableOutput();
    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(false, has_entered_braking_zone);
    ASSERT_EQ(true, has_reached_max_velocity);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << accelerating_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kCruising)
      << accelerating_cruising_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the Accelerating -> FailureBraking transition conditions are met and
   * verifies the behaviour.
   */
  void testAcceleratingEmergency()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kAccelerating);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Enforce Accelerating -> FailureBraking
    forceEmergency();

    // Prevent FailureBraking -> FailureStopped
    nav_data.velocity = 100;

    // Verify transition conditions are as intended
    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_stopped   = checkPodStopped(log, nav_data);

    enableOutput();
    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_stopped);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << accelerating_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kEmergencyBraking)
      << accelerating_failure_transition_error;
    disableOutput();
  }

  void testCruisingToNominalBraking()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kCruising);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Prevent Cruising -> FailureBraking
    embrakes_data.module_status           = ModuleStatus::kReady;
    nav_data.module_status                = ModuleStatus::kReady;
    telemetry_data.module_status          = ModuleStatus::kReady;
    motors_data.module_status             = ModuleStatus::kReady;
    sensors_data.module_status            = ModuleStatus::kReady;
    batteries_data.module_status          = ModuleStatus::kReady;
    telemetry_data.emergency_stop_command = false;

    // Enforce Cruising -> NominalBraking
    nav_data.braking_distance = 1000;
    nav_data.displacement     = Navigation::kRunLength - nav_data.braking_distance;

    // Prevent NominalBraking -> Finished
    nav_data.velocity = 100;

    // Verify transition conditions are as intended
    bool has_emergency            = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_entered_braking_zone = checkEnteredBrakingZone(log, nav_data);
    bool has_stopped              = checkPodStopped(log, nav_data);

    enableOutput();
    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_entered_braking_zone);
    ASSERT_EQ(false, has_stopped);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << accelerating_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kNominalBraking)
      << cruising_braking_transition_error;
    disableOutput();
  }

  void testCruisingEmergency()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kCruising);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Enforce Cruising -> FailureBraking
    forceEmergency();

    // Prevent FailureBraking -> FailureStopped
    nav_data.velocity = 100;

    // Verify transition conditions are as intended
    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_stopped   = checkPodStopped(log, nav_data);

    enableOutput();
    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_stopped);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << accelerating_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kEmergencyBraking)
      << accelerating_failure_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the NominalBraking -> Finished transition conditions are met and
   * verifies the behaviour.
   */
  void testNominalBrakingToFinished()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kNominalBraking);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Prevent NominalBraking -> FailureBraking
    embrakes_data.module_status           = ModuleStatus::kReady;
    nav_data.module_status                = ModuleStatus::kReady;
    telemetry_data.module_status          = ModuleStatus::kReady;
    motors_data.module_status             = ModuleStatus::kReady;
    sensors_data.module_status            = ModuleStatus::kReady;
    batteries_data.module_status          = ModuleStatus::kReady;
    telemetry_data.emergency_stop_command = false;

    // Enforce NominalBraking -> Finished
    nav_data.velocity = 0;

    // Prevent Finished -> Off
    telemetry_data.shutdown_command = false;

    // Verify transition conditions are as intended
    bool has_emergency        = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_stopped          = checkPodStopped(log, nav_data);
    bool has_shutdown_command = checkShutdownCommand(log, telemetry_data);

    enableOutput();
    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_stopped);
    ASSERT_EQ(false, has_shutdown_command);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << nominal_braking_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFinished)
      << braking_finished_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the NominalBraking -> FailureBraking transition conditions are met and
   * verifies the behaviour.
   */
  void testNominalBrakingEmergency()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kNominalBraking);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Enforce NominalBraking -> FailureBraking
    forceEmergency();

    // Prevent FailureBraking -> FailureStopped
    nav_data.velocity = 100;

    // Verify transition conditions are as intended
    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                        telemetry_data, sensors_data, motors_data);
    bool has_stopped   = checkPodStopped(log, nav_data);

    enableOutput();
    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_stopped);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.critical_failure, false) << nominal_braking_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kEmergencyBraking)
      << braking_failure_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the Finished -> Off transition conditions are met and
   * verifies the behaviour.
   */
  void testFinishedToOff()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFinished);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Enforce Finished -> Off
    telemetry_data.shutdown_command = true;

    // Verify transition conditions are as intended
    bool has_shutdown_command = checkShutdownCommand(log, telemetry_data);

    enableOutput();
    ASSERT_EQ(true, has_shutdown_command);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    System &sys = System::getSystem();
    ASSERT_EQ(sys.running_, false) << finished_off_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the FailureBraking -> Stopped transition conditions are met and
   * verifies the behaviour.
   */
  void testFailureBrakingToStopped()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kEmergencyBraking);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Enforce FailureBraking -> FailureStopped
    nav_data.velocity = 0;

    // Prevent FailureStopped -> Off
    telemetry_data.shutdown_command = false;

    // Verify transition conditions are as intended
    bool has_stopped          = checkPodStopped(log, nav_data);
    bool has_shutdown_command = checkShutdownCommand(log, telemetry_data);

    enableOutput();
    ASSERT_EQ(true, has_stopped);
    ASSERT_EQ(false, has_shutdown_command);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFailureStopped)
      << failure_braking_stopped_transition_error;
    disableOutput();
  }

  /**
   * Modifies data such that the FailureStopped -> Off transition conditions are met and
   * verifies the behaviour.
   */
  void testFailureStoppedToOff()
  {
    // Check initial state
    readData();
    enableOutput();
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFailureStopped);
    disableOutput();

    // Randomise data
    randomiseInternally();

    // Enforce FailureStopped -> Off
    telemetry_data.shutdown_command = true;

    // Verify transition conditions are as intended
    bool has_shutdown_command = checkShutdownCommand(log, telemetry_data);

    enableOutput();
    ASSERT_EQ(true, has_shutdown_command);
    disableOutput();

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    enableOutput();
    System &sys = System::getSystem();
    ASSERT_EQ(sys.running_, false) << finished_off_transition_error;
    disableOutput();
  }

 protected:
  void SetUp() { disableOutput(); }

  void TearDown() { enableOutput(); }
};

/**
 * Verifies the nominal run behaviour without any emergencies and without the Cruising state.
 */
TEST_F(RunTest, nominalRunWithoutCruising)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    System &sys  = System::getSystem();
    sys.running_ = true;

    initialiseData();

    Thread *state_machine = new Main(0, stm_log);
    state_machine->start();

    waitForUpdate();

    testIdleToCalibrating();
    testCalibratingToReady();
    testReadyToAccelerating();
    testAcceleratingToNominalBraking();
    testNominalBrakingToFinished();
    testFinishedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the nominal run behaviour without any emergencies but with the Cruising state.
 */
TEST_F(RunTest, nominalRunWithCruising)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    System &sys  = System::getSystem();
    sys.running_ = true;

    initialiseData();

    Thread *state_machine = new Main(0, stm_log);
    state_machine->start();

    waitForUpdate();

    testIdleToCalibrating();
    testCalibratingToReady();
    testReadyToAccelerating();
    testAcceleratingToCruising();
    testCruisingToNominalBraking();
    testNominalBrakingToFinished();
    testFinishedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Idle.
 */
TEST_F(RunTest, idleEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    System &sys  = System::getSystem();
    sys.running_ = true;

    initialiseData();

    Thread *state_machine = new Main(0, stm_log);
    state_machine->start();

    waitForUpdate();

    testIdleEmergency();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Calibrating.
 */
TEST_F(RunTest, calibratingEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    System &sys  = System::getSystem();
    sys.running_ = true;

    initialiseData();

    Thread *state_machine = new Main(0, stm_log);
    state_machine->start();

    waitForUpdate();

    testIdleToCalibrating();
    testCalibratingEmergency();
    testFailureStoppedToOff();
    state_machine->join();

    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Ready.
 */
TEST_F(RunTest, readyEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    System &sys  = System::getSystem();
    sys.running_ = true;

    initialiseData();

    Thread *state_machine = new Main(0, stm_log);
    state_machine->start();

    waitForUpdate();

    testIdleToCalibrating();
    testCalibratingToReady();
    testReadyEmergency();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Accelerating.
 */
TEST_F(RunTest, acceleratingEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    System &sys  = System::getSystem();
    sys.running_ = true;

    initialiseData();

    Thread *state_machine = new Main(0, stm_log);
    state_machine->start();

    waitForUpdate();

    testIdleToCalibrating();
    testCalibratingToReady();
    testReadyToAccelerating();
    testAcceleratingEmergency();
    testFailureBrakingToStopped();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Cruising.
 */
TEST_F(RunTest, cruisingEmergency)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    System &sys  = System::getSystem();
    sys.running_ = true;

    initialiseData();

    Thread *state_machine = new Main(0, stm_log);
    state_machine->start();

    waitForUpdate();

    testIdleToCalibrating();
    testCalibratingToReady();
    testReadyToAccelerating();
    testAcceleratingToCruising();
    testCruisingEmergency();
    testFailureBrakingToStopped();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Braking without the
 * Cruising state.
 */
TEST_F(RunTest, brakingEmergencyWithoutCruising)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    System &sys  = System::getSystem();
    sys.running_ = true;

    initialiseData();

    Thread *state_machine = new Main(0, stm_log);
    state_machine->start();

    waitForUpdate();

    testIdleToCalibrating();
    testCalibratingToReady();
    testReadyToAccelerating();
    testAcceleratingToNominalBraking();
    testNominalBrakingEmergency();
    testFailureBrakingToStopped();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Braking with the Cruising
 * state.
 */
TEST_F(RunTest, brakingEmergencyWithCruising)
{
  for (int i = 0; i < TEST_SIZE; i++) {
    System &sys  = System::getSystem();
    sys.running_ = true;

    initialiseData();

    Thread *state_machine = new Main(0, stm_log);
    state_machine->start();

    waitForUpdate();

    testIdleToCalibrating();
    testCalibratingToReady();
    testReadyToAccelerating();
    testAcceleratingToCruising();
    testCruisingToNominalBraking();
    testNominalBrakingEmergency();
    testFailureBrakingToStopped();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}
