#include "randomiser.hpp"
#include "test.hpp"

#include <random>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <data/data.hpp>
#include <state_machine/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class RunTest : public Test {
 protected:
  static constexpr int kTestSize = 10;

  // ---- Data -----------------

  data::Data &data_ = data::Data::getInstance();

  data::EmergencyBrakes brakes_data_;
  data::Navigation nav_data_;
  data::Batteries batteries_data_;
  data::Telemetry telemetry_data_;
  data::Sensors sensors_data_;
  data::Motors motors_data_;
  data::StateMachine stm_data_;

  void randomiseInternally()
  {
    Randomiser::randomiseBrakes(brakes_data_);
    Randomiser::randomiseNavigation(nav_data_);
    Randomiser::randomiseTelemetry(telemetry_data_);
    Randomiser::randomiseMotors(motors_data_);
    Randomiser::randomiseSensorsData(sensors_data_);
    Randomiser::randomiseBatteriesData(batteries_data_);
  }

  /**
   *  This method is used to force the emergency case.
   */
  void forceEmergency()
  {
    // We only need to set one critical failure as other behaviour
    // is tested in transitions.test.cpp.
    brakes_data_.module_status = data::ModuleStatus::kCriticalFailure;
  }

  /**
   *  The updated data is written.
   */
  void writeData()
  {
    data_.setEmergencyBrakesData(brakes_data_);
    data_.setNavigationData(nav_data_);
    data_.setTelemetryData(telemetry_data_);
    data_.setMotorData(motors_data_);
    data_.setSensorsData(sensors_data_);
    data_.setBatteriesData(batteries_data_);
  }

  /**
   *  The written data is read.
   */
  void readData()
  {
    brakes_data_    = data_.getEmergencyBrakesData();
    stm_data_       = data_.getStateMachineData();
    nav_data_       = data_.getNavigationData();
    telemetry_data_ = data_.getTelemetryData();
    motors_data_    = data_.getMotorData();
    sensors_data_   = data_.getSensorsData();
    batteries_data_ = data_.getBatteriesData();
  }

  /**
   * Allows the state machine thread to process the central data structure and transition between
   * states.
   *
   * If tests are failing for no apparent reason, try increasing the sleep duration to be sure this
   * is not the source of the problem.
   */
  void waitForUpdate() { utils::concurrent::Thread::sleep(20); }

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
    telemetry_data_.emergency_stop_command = false;

    // Prevent Idle -> Calibrating
    brakes_data_.module_status        = data::ModuleStatus::kStart;
    nav_data_.module_status           = data::ModuleStatus::kStart;
    telemetry_data_.module_status     = data::ModuleStatus::kStart;
    motors_data_.module_status        = data::ModuleStatus::kStart;
    sensors_data_.module_status       = data::ModuleStatus::kStart;
    batteries_data_.module_status     = data::ModuleStatus::kStart;
    telemetry_data_.calibrate_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_calibrate_command   = state_machine::checkCalibrateCommand(telemetry_data_);
    const bool has_modules_initialised = state_machine::checkModulesInitialised(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(false, has_calibrate_command);
    ASSERT_EQ(false, has_modules_initialised);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();
  }

  /**
   * Modifies data such that the Idle -> PreCalibrating transition conditions are met and verifies
   * the behaviour.
   */

  void testIdleToPreCalibrating()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kIdle);

    // Randomise data
    randomiseInternally();

    // Prevent Idle -> FailureStopped
    telemetry_data_.emergency_stop_command = false;

    // Enforce Idle -> PreCalibrating
    telemetry_data_.calibrate_command = false;
    brakes_data_.module_status        = data::ModuleStatus::kInit;
    nav_data_.module_status           = data::ModuleStatus::kInit;
    telemetry_data_.module_status     = data::ModuleStatus::kInit;
    motors_data_.module_status        = data::ModuleStatus::kInit;
    sensors_data_.module_status       = data::ModuleStatus::kInit;
    batteries_data_.module_status     = data::ModuleStatus::kInit;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_calibrating_command = state_machine::checkCalibrateCommand(telemetry_data_);
    const bool has_modules_initialised = state_machine::checkModulesInitialised(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_modules_ready = state_machine::checkModulesReady(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(false, has_calibrating_command);
    ASSERT_EQ(true, has_modules_initialised);
    ASSERT_EQ(false, has_modules_ready);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Idle";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kPreCalibrating)
      << "failed to transition from Idle to PreCalibrating";
  }

  /**
   * Modifies data such that the Idle -> FailureStopped transition conditions are met and verifies
   * the behaviour.
   */
  void testIdleEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kIdle);

    // Randomise data
    randomiseInternally();

    // Enforce Idle -> FailureStopped
    forceEmergency();

    // Prevent FailureStopped -> Off
    telemetry_data_.shutdown_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_shutdown_command = state_machine::checkShutdownCommand(telemetry_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_shutdown_command);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Idle";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailureStopped)
      << "failed to transition from Idle to FailureStopped";
  }

  /**
   * Modifies data such that the PreCalibrating -> Calibrating transition conditions are met and
   * verifies the behaviour.
   */

  void testPreCalibratingToCalibrating()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kPreCalibrating);

    // Randomise data
    randomiseInternally();

    // Prevent Idle -> FailureStopped
    telemetry_data_.emergency_stop_command = false;

    // Enforce Idle -> PreCalibrating
    telemetry_data_.calibrate_command = true;
    brakes_data_.module_status        = data::ModuleStatus::kInit;
    nav_data_.module_status           = data::ModuleStatus::kInit;
    telemetry_data_.module_status     = data::ModuleStatus::kInit;
    motors_data_.module_status        = data::ModuleStatus::kInit;
    sensors_data_.module_status       = data::ModuleStatus::kInit;
    batteries_data_.module_status     = data::ModuleStatus::kInit;

    // Prevent Calibrating -> Ready
    // >> No work required due to the above

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_calibrating_command = state_machine::checkCalibrateCommand(telemetry_data_);
    const bool has_modules_initialised = state_machine::checkModulesInitialised(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_modules_ready = state_machine::checkModulesReady(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_calibrating_command);
    ASSERT_EQ(true, has_modules_initialised);
    ASSERT_EQ(false, has_modules_ready);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in PreCalibrating";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kCalibrating)
      << "failed to transition from PreCalibrating to Calibrating";
  }

  /**
   * Modifies data such that the Idle -> FailureStopped transition conditions are met and verifies
   * the behaviour.
   */
  void testPreCalibratingEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kPreCalibrating);

    // Randomise data
    randomiseInternally();

    // Enforce Idle -> FailureStopped
    forceEmergency();

    // Prevent FailureStopped -> Off
    telemetry_data_.shutdown_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_shutdown_command = state_machine::checkShutdownCommand(telemetry_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_shutdown_command);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in PreCalibrating";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailureStopped)
      << "failed to transition from PreCalibrating to FailureStopped";
  }

  /**
   * Modifies data such that the Calibrating -> Ready transition conditions are met and verifies
   * the behaviour.
   */
  void testCalibratingToReady()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kCalibrating);

    // Randomise data
    randomiseInternally();

    // Prevent Calibrating -> FailureStopped
    telemetry_data_.emergency_stop_command = false;

    // Enforce Calibrating -> Ready
    brakes_data_.module_status    = data::ModuleStatus::kReady;
    nav_data_.module_status       = data::ModuleStatus::kReady;
    telemetry_data_.module_status = data::ModuleStatus::kReady;
    motors_data_.module_status    = data::ModuleStatus::kReady;
    sensors_data_.module_status   = data::ModuleStatus::kReady;
    batteries_data_.module_status = data::ModuleStatus::kReady;

    // Prevent Ready -> Accelerating
    telemetry_data_.launch_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_modules_ready = state_machine::checkModulesReady(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_launch_command = state_machine::checkLaunchCommand(telemetry_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_modules_ready);
    ASSERT_EQ(false, has_launch_command);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Calibrating";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kReady)
      << "failed to transition from Calibrating to Ready";
  }

  /**
   * Modifies data such that the Calibrating -> FailureStopped transition conditions are met and
   * verifies the behaviour.
   */
  void testCalibratingEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kCalibrating);

    // Randomise data
    randomiseInternally();

    // Enforce Calibrating -> FailureStopped
    forceEmergency();

    // Prevent FailureStopped -> Off
    telemetry_data_.shutdown_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_shutdown_command = state_machine::checkShutdownCommand(telemetry_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_shutdown_command);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Calibrating";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailureStopped)
      << "failed to transition from Calibrating to FailureStopped";
  }

  /**
   * Modifies data such that the Ready -> Accelerating transition conditions are met and
   * verifies the behaviour.
   */
  void testReadyToAccelerating()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kReady);

    // Randomise data
    randomiseInternally();

    // Prevent Ready -> FailureStopped
    brakes_data_.module_status             = data::ModuleStatus::kReady;
    nav_data_.module_status                = data::ModuleStatus::kReady;
    telemetry_data_.module_status          = data::ModuleStatus::kReady;
    motors_data_.module_status             = data::ModuleStatus::kReady;
    sensors_data_.module_status            = data::ModuleStatus::kReady;
    batteries_data_.module_status          = data::ModuleStatus::kReady;
    telemetry_data_.emergency_stop_command = false;

    // Enforce Ready -> Accelerating
    telemetry_data_.launch_command = true;

    // Prevent Accelerating -> NominalBraking
    nav_data_.displacement     = 0;
    nav_data_.braking_distance = 0;

    // Prevent Accelerating -> Cruising
    nav_data_.velocity = data::Navigation::kMaximumVelocity / 2;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_launch_command       = state_machine::checkLaunchCommand(telemetry_data_);
    const bool has_entered_braking_zone = state_machine::checkEnteredBrakingZone(log_, nav_data_);
    const bool has_reached_max_velocity = state_machine::checkReachedMaxVelocity(log_, nav_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_launch_command);
    ASSERT_EQ(false, has_entered_braking_zone);
    ASSERT_EQ(false, has_reached_max_velocity);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Ready";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kAccelerating)
      << "failed to transition from Ready to Accelerating";
  }

  /**
   * Modifies data such that the Ready -> FailureStopped transition conditions are met and
   * verifies the behaviour.
   */
  void testReadyEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kReady);

    // Randomise data
    randomiseInternally();

    // Enforce Ready -> FailureStopped
    forceEmergency();

    // Prevent FailureStopped -> Off
    telemetry_data_.shutdown_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_shutdown_command = state_machine::checkShutdownCommand(telemetry_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_shutdown_command);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered critical failure in Ready";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailureStopped)
      << "failed to transition from Ready to FailureStopped";
  }

  /**
   * Modifies data such that the Accelerating -> NominalBraking transition conditions are met and
   * verifies the behaviour.
   */
  void testAcceleratingToNominalBraking()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kAccelerating);

    // Randomise data
    randomiseInternally();

    // Prevent Accelerating -> FailureBraking
    brakes_data_.module_status             = data::ModuleStatus::kReady;
    nav_data_.module_status                = data::ModuleStatus::kReady;
    telemetry_data_.module_status          = data::ModuleStatus::kReady;
    motors_data_.module_status             = data::ModuleStatus::kReady;
    sensors_data_.module_status            = data::ModuleStatus::kReady;
    batteries_data_.module_status          = data::ModuleStatus::kReady;
    telemetry_data_.emergency_stop_command = false;

    // Enforce Accelerating -> NominalBraking
    nav_data_.braking_distance = 1000;
    nav_data_.displacement     = data::Navigation::kRunLength - nav_data_.braking_distance;

    // Prevent NominalBraking -> Finished
    nav_data_.velocity = 100;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_entered_braking_zone = state_machine::checkEnteredBrakingZone(log_, nav_data_);
    const bool has_stopped              = state_machine::checkPodStopped(log_, nav_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_entered_braking_zone);
    ASSERT_EQ(false, has_stopped);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Accelerating";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kNominalBraking)
      << "failed to transition from Accelerating to NominalBraking";
  }

  /**
   * Modifies data_ such that the Accelerating -> Cruising transition conditions are met and
   * verifies the behaviour.
   */
  void testAcceleratingToCruising()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kAccelerating);

    // Randomise data_
    randomiseInternally();

    // Prevent Accelerating -> FailureBraking
    brakes_data_.module_status             = data::ModuleStatus::kReady;
    nav_data_.module_status                = data::ModuleStatus::kReady;
    telemetry_data_.module_status          = data::ModuleStatus::kReady;
    motors_data_.module_status             = data::ModuleStatus::kReady;
    sensors_data_.module_status            = data::ModuleStatus::kReady;
    batteries_data_.module_status          = data::ModuleStatus::kReady;
    telemetry_data_.emergency_stop_command = false;

    // Prevent Accelerating -> NominalBraking
    // Prevent Cruising -> NominalBraking
    nav_data_.braking_distance = 0;
    nav_data_.displacement     = 0;

    // Enforce Accelerating -> Cruising
    nav_data_.velocity = data::Navigation::kMaximumVelocity;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_entered_braking_zone = state_machine::checkEnteredBrakingZone(log_, nav_data_);
    const bool has_reached_max_velocity = state_machine::checkReachedMaxVelocity(log_, nav_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(false, has_entered_braking_zone);
    ASSERT_EQ(true, has_reached_max_velocity);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Accelerating";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kCruising)
      << "failed to transition from Accelerating to Cruising";
  }

  /**
   * Modifies data_ such that the Accelerating -> FailureBraking transition conditions are met and
   * verifies the behaviour.
   */
  void testAcceleratingEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kAccelerating);

    // Randomise data
    randomiseInternally();

    // Enforce Accelerating -> FailureBraking
    forceEmergency();

    // Prevent FailureBraking -> FailureStopped
    nav_data_.velocity = 100;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_stopped = state_machine::checkPodStopped(log_, nav_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_stopped);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Accelerating";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kEmergencyBraking)
      << "failed to transition from Accelerating to EmergencyBraking";
  }

  void testCruisingToNominalBraking()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kCruising);

    // Randomise data
    randomiseInternally();

    // Prevent Cruising -> FailureBraking
    brakes_data_.module_status             = data::ModuleStatus::kReady;
    nav_data_.module_status                = data::ModuleStatus::kReady;
    telemetry_data_.module_status          = data::ModuleStatus::kReady;
    motors_data_.module_status             = data::ModuleStatus::kReady;
    sensors_data_.module_status            = data::ModuleStatus::kReady;
    batteries_data_.module_status          = data::ModuleStatus::kReady;
    telemetry_data_.emergency_stop_command = false;

    // Enforce Cruising -> NominalBraking
    nav_data_.braking_distance = 1000;
    nav_data_.displacement     = data::Navigation::kRunLength - nav_data_.braking_distance;

    // Prevent NominalBraking -> Finished
    nav_data_.velocity = 100;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_entered_braking_zone = state_machine::checkEnteredBrakingZone(log_, nav_data_);
    const bool has_stopped              = state_machine::checkPodStopped(log_, nav_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_entered_braking_zone);
    ASSERT_EQ(false, has_stopped);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Cruising";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kNominalBraking)
      << "failed to transition from Cruising to NominalBraking";
  }

  void testCruisingEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kCruising);

    // Randomise data
    randomiseInternally();

    // Enforce Cruising -> FailureBraking
    forceEmergency();

    // Prevent FailureBraking -> FailureStopped
    nav_data_.velocity = 100;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_stopped = state_machine::checkPodStopped(log_, nav_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_stopped);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Cruising";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kEmergencyBraking)
      << "failed to transition from Cruising to EmergencyBraking";
  }

  /**
   * Modifies data such that the NominalBraking -> Finished transition conditions are met and
   * verifies the behaviour.
   */
  void testNominalBrakingToFinished()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kNominalBraking);

    // Randomise data
    randomiseInternally();

    // Prevent NominalBraking -> FailureBraking
    brakes_data_.module_status             = data::ModuleStatus::kReady;
    nav_data_.module_status                = data::ModuleStatus::kReady;
    telemetry_data_.module_status          = data::ModuleStatus::kReady;
    motors_data_.module_status             = data::ModuleStatus::kReady;
    sensors_data_.module_status            = data::ModuleStatus::kReady;
    batteries_data_.module_status          = data::ModuleStatus::kReady;
    telemetry_data_.emergency_stop_command = false;

    // Enforce NominalBraking -> Finished
    nav_data_.velocity = 0;

    // Prevent Finished -> Off
    telemetry_data_.shutdown_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_stopped          = state_machine::checkPodStopped(log_, nav_data_);
    const bool has_shutdown_command = state_machine::checkShutdownCommand(telemetry_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_stopped);
    ASSERT_EQ(false, has_shutdown_command);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in NominalBraking";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFinished)
      << "failed to transition from NominalBraking to Finished";
  }

  /**
   * Modifies data such that the NominalBraking -> FailureBraking transition conditions are met and
   * verifies the behaviour.
   */
  void testNominalBrakingEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kNominalBraking);

    // Randomise data
    randomiseInternally();

    // Enforce NominalBraking -> FailureBraking
    forceEmergency();

    // Prevent FailureBraking -> FailureStopped
    nav_data_.velocity = 100;

    // Verify transition conditions are as intended
    const bool has_emergency = state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_stopped = state_machine::checkPodStopped(log_, nav_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_stopped);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in NominalBraking";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kEmergencyBraking)
      << "failed to transition from NominalBraking to EmergencyBraking";
  }

  /**
   * Modifies data such that the Finished -> Off transition conditions are met and
   * verifies the behaviour.
   */
  void testFinishedToOff()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFinished);

    // Randomise data
    randomiseInternally();

    // Enforce Finished -> Off
    telemetry_data_.shutdown_command = true;

    // Verify transition conditions are as intended
    const bool has_shutdown_command = state_machine::checkShutdownCommand(telemetry_data_);

    ASSERT_EQ(true, has_shutdown_command);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    utils::System &sys = utils::System::getSystem();
    ASSERT_EQ(sys.isRunning(), false) << "failed to transition from Finished to Off";
  }

  /**
   * Modifies data such that the FailureBraking -> Stopped transition conditions are met and
   * verifies the behaviour.
   */
  void testFailureBrakingToStopped()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kEmergencyBraking);

    // Randomise data
    randomiseInternally();

    // Enforce FailureBraking -> FailureStopped
    nav_data_.velocity = 0;

    // Prevent FailureStopped -> Off
    telemetry_data_.shutdown_command = false;

    // Verify transition conditions are as intended
    const bool has_stopped          = state_machine::checkPodStopped(log_, nav_data_);
    const bool has_shutdown_command = state_machine::checkShutdownCommand(telemetry_data_);

    ASSERT_EQ(true, has_stopped);
    ASSERT_EQ(false, has_shutdown_command);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailureStopped)
      << "failed to transition from FailureBraking to FailureStopped";
  }

  /**
   * Modifies data such that the FailureStopped -> Off transition conditions are met and
   * verifies the behaviour.
   */
  void testFailureStoppedToOff()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailureStopped);

    // Randomise data
    randomiseInternally();

    // Enforce FailureStopped -> Off
    telemetry_data_.shutdown_command = true;

    // Verify transition conditions are as intended
    const bool has_shutdown_command = state_machine::checkShutdownCommand(telemetry_data_);

    ASSERT_EQ(true, has_shutdown_command);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    utils::System &sys = utils::System::getSystem();
    ASSERT_EQ(sys.isRunning(), false) << "failed to transition from FailureStopped to Off";
  }
};

/**
 * Verifies the nominal run behaviour without any emergencies and without the Cruising state.
 */
TEST_F(RunTest, nominalRunWithoutCruising)
{
  static const char *args[] = {kDefaultConfigPath.c_str()};
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(1, args);

    initialiseData();

    utils::concurrent::Thread *state_machine = new state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
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
  static const char *args[] = {kDefaultConfigPath.c_str()};
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(1, args);

    initialiseData();

    utils::concurrent::Thread *state_machine = new state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
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
  static const char *args[] = {kDefaultConfigPath.c_str()};
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(1, args);

    initialiseData();

    utils::concurrent::Thread *state_machine = new state_machine::Main();
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
  static const char *args[] = {kDefaultConfigPath.c_str()};
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(1, args);

    initialiseData();

    utils::concurrent::Thread *state_machine = new state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
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
  static const char *args[] = {kDefaultConfigPath.c_str()};
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(1, args);

    initialiseData();

    utils::concurrent::Thread *state_machine = new state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
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
  static const char *args[] = {kDefaultConfigPath.c_str()};
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(1, args);

    initialiseData();

    utils::concurrent::Thread *state_machine = new state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
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
  static const char *args[] = {kDefaultConfigPath.c_str()};
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(1, args);

    initialiseData();

    utils::concurrent::Thread *state_machine = new state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
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
  static const char *args[] = {kDefaultConfigPath.c_str()};
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(1, args);

    initialiseData();

    utils::concurrent::Thread *state_machine = new state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
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
  static const char *args[] = {kDefaultConfigPath.c_str()};
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(1, args);

    initialiseData();

    utils::concurrent::Thread *state_machine = new state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
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

}  // namespace hyped::testing
