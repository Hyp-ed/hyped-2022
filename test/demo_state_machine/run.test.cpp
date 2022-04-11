#include "demorandomiser.hpp"
#include "test.hpp"

#include <random>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <data/data.hpp>
#include <demo_state_machine/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::testing {

class DemoRunTest : public Test {
 protected:
  static constexpr int kTestSize = 10;

  // ---- Data -----------------

  data::Data &data_ = data::Data::getInstance();

  data::Brakes brakes_data_;
  data::Navigation nav_data_;
  data::FullBatteryData batteries_data_;
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
    data_.setBrakesData(brakes_data_);
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
    brakes_data_    = data_.getBrakesData();
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
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_calibrate_command   = demo_state_machine::checkCalibrateCommand(telemetry_data_);
    const bool has_modules_initialised = demo_state_machine::checkModulesInitialised(
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
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_calibrating_command = demo_state_machine::checkCalibrateCommand(telemetry_data_);
    const bool has_modules_initialised = demo_state_machine::checkModulesInitialised(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_modules_ready = demo_state_machine::checkModulesReady(
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
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_shutdown_command = demo_state_machine::checkShutdownCommand(telemetry_data_);

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

    // Prevent PreCalibrating -> FailureStopped
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
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_calibrating_command = demo_state_machine::checkCalibrateCommand(telemetry_data_);
    const bool has_modules_initialised = demo_state_machine::checkModulesInitialised(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_modules_ready = demo_state_machine::checkModulesReady(
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
   * Modifies data such that the PreCalibrating -> FailureStopped transition conditions are met and
   * verifies the behaviour.
   */
  void testPreCalibratingEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kPreCalibrating);

    // Randomise data
    randomiseInternally();

    // Enforce PreCalibrating -> FailureStopped
    forceEmergency();

    // Prevent FailureStopped -> Off
    telemetry_data_.shutdown_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_shutdown_command = demo_state_machine::checkShutdownCommand(telemetry_data_);

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
   * Modifies data such that the Calibrating -> PreReady transition conditions are met and verifies
   * the behaviour.
   */
  void testCalibratingToPreReady()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kCalibrating);

    // Randomise data
    randomiseInternally();

    // Prevent Calibrating -> FailureStopped
    telemetry_data_.emergency_stop_command = false;

    // Enforce Calibrating -> PreReady
    brakes_data_.module_status    = data::ModuleStatus::kReady;
    nav_data_.module_status       = data::ModuleStatus::kReady;
    telemetry_data_.module_status = data::ModuleStatus::kReady;
    motors_data_.module_status    = data::ModuleStatus::kReady;
    sensors_data_.module_status   = data::ModuleStatus::kReady;
    batteries_data_.module_status = data::ModuleStatus::kReady;

    // Preventing PreReady -> Ready
    sensors_data_.high_power_off = true;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_modules_ready = demo_state_machine::checkModulesReady(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_high_power_on = !demo_state_machine::checkHighPowerOff(sensors_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_modules_ready);
    ASSERT_EQ(false, has_high_power_on);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Calibrating";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kPreReady)
      << "failed to transition from Calibrating to PreReady";
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
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_shutdown_command = demo_state_machine::checkShutdownCommand(telemetry_data_);

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
   * Modifies data such that the PreReady -> Ready transition conditions are met and verifies
   * the behaviour.
   */
  void testPreReadyToReady()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kPreReady);

    // Randomise data
    randomiseInternally();

    // Prevent PreReady -> FailureStopped
    telemetry_data_.emergency_stop_command = false;

    // Enforce PreReady-> Ready
    brakes_data_.module_status    = data::ModuleStatus::kReady;
    nav_data_.module_status       = data::ModuleStatus::kReady;
    telemetry_data_.module_status = data::ModuleStatus::kReady;
    motors_data_.module_status    = data::ModuleStatus::kReady;
    sensors_data_.module_status   = data::ModuleStatus::kReady;
    batteries_data_.module_status = data::ModuleStatus::kReady;

    // Prevent Ready -> Accelerating
    telemetry_data_.launch_command = false;

    // Asserting PreReady -> Ready
    sensors_data_.high_power_off = false;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_modules_ready = demo_state_machine::checkModulesReady(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_launch_command = demo_state_machine::checkLaunchCommand(telemetry_data_);
    const bool has_high_power_on  = !demo_state_machine::checkHighPowerOff(sensors_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_modules_ready);
    ASSERT_EQ(true, has_high_power_on);
    ASSERT_EQ(false, has_launch_command);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in PreReady";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kReady)
      << "failed to transition from PreReady to Ready";
  }

  /**
   * Modifies data such that the PreReady -> FailureStopped transition conditions are met and
   * verifies the behaviour.
   */
  void testPreReadyEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kPreReady);

    // Randomise data
    randomiseInternally();

    // Enforce PreReady -> FailureStopped
    forceEmergency();

    // Prevent FailureStopped -> Off
    telemetry_data_.shutdown_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_shutdown_command = demo_state_machine::checkShutdownCommand(telemetry_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_shutdown_command);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in PreReady";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailureStopped)
      << "failed to transition from PreReady to FailureStopped";
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
    sensors_data_.high_power_off   = false;

    // Prevent Accelerating -> NominalBraking
    telemetry_data_.emergency_stop_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_launch_command = demo_state_machine::checkLaunchCommand(telemetry_data_);
    const bool has_received_braking_command
      = demo_state_machine::checkBrakingCommand(telemetry_data_);
    const bool has_high_power_off = demo_state_machine::checkHighPowerOff(sensors_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_launch_command);
    ASSERT_EQ(false, has_received_braking_command);
    ASSERT_EQ(false, has_high_power_off);

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

    // Prevent Accelerating -> NominalBraking
    telemetry_data_.emergency_stop_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_shutdown_command = demo_state_machine::checkShutdownCommand(telemetry_data_);

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
   * Modifies data such that the Accelerating -> PreBraking transition conditions are met and
   * verifies the behaviour.
   */
  void testAcceleratingToPreBraking()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kAccelerating);

    // Randomise data
    randomiseInternally();

    // Prevent Accelerating -> FailureBraking
    brakes_data_.module_status    = data::ModuleStatus::kReady;
    nav_data_.module_status       = data::ModuleStatus::kReady;
    telemetry_data_.module_status = data::ModuleStatus::kReady;
    motors_data_.module_status    = data::ModuleStatus::kReady;
    sensors_data_.module_status   = data::ModuleStatus::kReady;
    batteries_data_.module_status = data::ModuleStatus::kReady;

    // Enforcing Accelerating -> PreBraking
    telemetry_data_.emergency_stop_command = true;
    nav_data_.velocity                     = 69;

    // Prevent PreBraking -> NominalBraking
    sensors_data_.high_power_off = false;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_received_braking_command
      = demo_state_machine::checkBrakingCommand(telemetry_data_);
    const bool has_stopped        = demo_state_machine::checkPodStopped(log_, nav_data_);
    const bool has_high_power_off = demo_state_machine::checkHighPowerOff(sensors_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(false, has_stopped);
    ASSERT_EQ(true, has_received_braking_command);
    ASSERT_EQ(false, has_high_power_off);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Accelerating";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kPreBraking)
      << "failed to transition from Accelerating to PreBraking";
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

    // Prevent Cruising -> PreBraking
    sensors_data_.high_power_off = false;

    // Enforcing Accelerting -> Cruising
    utils::concurrent::Thread::sleep(120);  // 0.12s

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_received_braking_command
      = demo_state_machine::checkBrakingCommand(telemetry_data_);
    const bool has_high_power_off = demo_state_machine::checkHighPowerOff(sensors_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(false, has_received_braking_command);
    ASSERT_EQ(false, has_high_power_off);

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
   * Modifies data_ such that the Accelerating -> FailurePreBraking transition conditions are met
   * and verifies the behaviour.
   */
  void testAcceleratingEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kAccelerating);

    // Randomise data
    randomiseInternally();

    // Enforce Accelerating -> FailurePreBraking
    forceEmergency();

    // Prevent FailurePreBraking -> FailureStopped
    nav_data_.velocity = 100;

    // Preventing Accelerating -> PreBraking
    telemetry_data_.emergency_stop_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_stopped        = demo_state_machine::checkPodStopped(log_, nav_data_);
    const bool has_high_power_off = demo_state_machine::checkHighPowerOff(sensors_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_stopped);
    ASSERT_EQ(false, has_high_power_off);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Accelerating";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailurePreBraking)
      << "failed to transition from Accelerating to FailurePreBraking";
  }

  /**
   * Modifies data_ such that the Cruising -> PreBraking transition conditions are met and
   * verifies the behaviour.
   */
  void testCruisingToPreBraking()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kCruising);

    // Randomise data
    randomiseInternally();

    // Prevent Cruising -> FailureBraking
    brakes_data_.module_status    = data::ModuleStatus::kReady;
    nav_data_.module_status       = data::ModuleStatus::kReady;
    telemetry_data_.module_status = data::ModuleStatus::kReady;
    motors_data_.module_status    = data::ModuleStatus::kReady;
    sensors_data_.module_status   = data::ModuleStatus::kReady;
    batteries_data_.module_status = data::ModuleStatus::kReady;

    // Enforcing Cruising -> PreBraking
    telemetry_data_.emergency_stop_command = true;

    // Prevent PreBraking -> NominalBraking
    sensors_data_.high_power_off = false;

    // Prevent PreBraking -> Finished
    nav_data_.velocity = 100;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_received_braking_command
      = demo_state_machine::checkBrakingCommand(telemetry_data_);
    const bool has_stopped        = demo_state_machine::checkPodStopped(log_, nav_data_);
    const bool has_high_power_off = demo_state_machine::checkHighPowerOff(sensors_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_received_braking_command);
    ASSERT_EQ(false, has_stopped);
    ASSERT_EQ(false, has_high_power_off);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Cruising";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kPreBraking)
      << "failed to transition from Cruising to PreBraking";
  }

  /**
   * Modifies data_ such that the Cruising-> FailurePreBraking transition conditions are met and
   * verifies the behaviour.
   */
  void testCruisingEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kCruising);

    // Randomise data
    randomiseInternally();

    // Enforce Cruising -> FailurePreBraking
    forceEmergency();

    // Prevent FailurePreBraking -> FailureStopped
    nav_data_.velocity = 100;

    // Preventing Calibrating -> PreBraking
    telemetry_data_.emergency_stop_command = false;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_stopped        = demo_state_machine::checkPodStopped(log_, nav_data_);
    const bool has_high_power_off = demo_state_machine::checkHighPowerOff(sensors_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_stopped);
    ASSERT_EQ(false, has_high_power_off);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in Cruising";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailurePreBraking)
      << "failed to transition from Cruising to FailurePreBraking";
  }

  /**
   * Modifies data_ such that the PreBraking-> FailurePreBraking transition conditions are met and
   * verifies the behaviour.
   */
  void testPreBrakingEmergency()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kPreBraking);

    // Randomise data
    randomiseInternally();

    // Enforce PreBraking -> FailurePreBraking
    forceEmergency();

    // Prevent FailurePreBraking -> FailureStopped
    nav_data_.velocity = 100;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_stopped        = demo_state_machine::checkPodStopped(log_, nav_data_);
    const bool has_high_power_off = demo_state_machine::checkHighPowerOff(sensors_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_stopped);
    ASSERT_EQ(false, has_high_power_off);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in PreBraking";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailurePreBraking)
      << "failed to transition from PreBraking to FailurePreBraking";
  }

  /**
   * Modifies data_ such that the PreBraking -> NominalBraking transition conditions are met and
   * verifies the behaviour.
   */
  void testPreBrakingToNominalBraking()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kPreBraking);

    // Randomise data
    randomiseInternally();

    // Prevent PreBraking -> FailureBraking
    brakes_data_.module_status             = data::ModuleStatus::kReady;
    nav_data_.module_status                = data::ModuleStatus::kReady;
    telemetry_data_.module_status          = data::ModuleStatus::kReady;
    motors_data_.module_status             = data::ModuleStatus::kReady;
    sensors_data_.module_status            = data::ModuleStatus::kReady;
    batteries_data_.module_status          = data::ModuleStatus::kReady;
    telemetry_data_.emergency_stop_command = false;

    // Enforce PreBraking -> NominalBraking
    nav_data_.braking_distance   = 1000;
    nav_data_.displacement       = data::Navigation::kRunLength - nav_data_.braking_distance;
    sensors_data_.high_power_off = true;

    // Prevent NominalBraking -> Finished
    nav_data_.velocity = 100;

    // Verify transition conditions are as intended
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_entered_braking_zone
      = demo_state_machine::checkEnteredBrakingZone(log_, nav_data_);
    const bool has_stopped        = demo_state_machine::checkPodStopped(log_, nav_data_);
    const bool has_high_power_off = demo_state_machine::checkHighPowerOff(sensors_data_);

    ASSERT_EQ(false, has_emergency);
    ASSERT_EQ(true, has_entered_braking_zone);
    ASSERT_EQ(false, has_stopped);
    ASSERT_EQ(true, has_high_power_off);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in PreBraking";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kNominalBraking)
      << "failed to transition from PreBraking to NominalBraking";
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
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_stopped          = demo_state_machine::checkPodStopped(log_, nav_data_);
    const bool has_shutdown_command = demo_state_machine::checkShutdownCommand(telemetry_data_);

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
    const bool has_emergency = demo_state_machine::checkEmergency(
      log_, brakes_data_, nav_data_, batteries_data_, telemetry_data_, sensors_data_, motors_data_);
    const bool has_stopped = demo_state_machine::checkPodStopped(log_, nav_data_);

    ASSERT_EQ(true, has_emergency);
    ASSERT_EQ(false, has_stopped);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in NominalBraking";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailureBraking)
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
    const bool has_shutdown_command = demo_state_machine::checkShutdownCommand(telemetry_data_);

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
   * Modifies data_ such that the FailurePreBraking-> FailureBraking transition conditions are met
   * and verifies the behaviour.
   */
  void testFailurePreBrakingToFailureBraking()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailurePreBraking);

    // Randomise data
    randomiseInternally();

    // Prevent FailureBraking -> FailureStopped
    nav_data_.velocity = 100;

    // Enforce FailurePreBraking -> FailureBraking
    sensors_data_.high_power_off = true;

    // Verify transition conditions are as intended
    const bool has_stopped        = demo_state_machine::checkPodStopped(log_, nav_data_);
    const bool has_high_power_off = demo_state_machine::checkHighPowerOff(sensors_data_);

    ASSERT_EQ(false, has_stopped);
    ASSERT_EQ(true, has_high_power_off);

    // Let STM do its thing
    writeData();
    waitForUpdate();
    readData();

    // Check result
    ASSERT_EQ(stm_data_.critical_failure, false) << "encountered failure in FailurePreBraking";
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailureBraking)
      << "failed to transition from FailurePreBraking to FailureBraking";
  }

  /**
   * Modifies data such that the FailureBraking -> Stopped transition conditions are met and
   * verifies the behaviour.
   */
  void testFailureBrakingToStopped()
  {
    // Check initial state
    readData();
    ASSERT_EQ(stm_data_.current_state, hyped::data::State::kFailureBraking);

    // Randomise data
    randomiseInternally();

    // Enforce FailureBraking -> FailureStopped
    nav_data_.velocity = 0;

    // Prevent FailureStopped -> Off
    telemetry_data_.shutdown_command = false;

    // Verify transition conditions are as intended
    const bool has_stopped          = demo_state_machine::checkPodStopped(log_, nav_data_);
    const bool has_shutdown_command = demo_state_machine::checkShutdownCommand(telemetry_data_);

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
    const bool has_shutdown_command = demo_state_machine::checkShutdownCommand(telemetry_data_);

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
TEST_F(DemoRunTest, demoNominalRunWithoutCruising)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
    testCalibratingToPreReady();
    testPreReadyToReady();
    testReadyToAccelerating();
    testAcceleratingToPreBraking();
    testPreBrakingToNominalBraking();
    testNominalBrakingToFinished();
    testFinishedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the nominal run behaviour without any emergencies but with the Cruising state.
 */
TEST_F(DemoRunTest, demoNominalRunWithCruising)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
    testCalibratingToPreReady();
    testPreReadyToReady();
    testReadyToAccelerating();
    testAcceleratingToCruising();
    testCruisingToPreBraking();
    testPreBrakingToNominalBraking();
    testNominalBrakingToFinished();
    testFinishedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Idle.
 */
TEST_F(DemoRunTest, demoIdleEmergency)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleEmergency();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in PreCalibrating.
 */
TEST_F(DemoRunTest, demoPreCalibratingEmergency)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingEmergency();
    testFailureStoppedToOff();
    state_machine->join();

    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Calibrating.
 */
TEST_F(DemoRunTest, demoCalibratingEmergency)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
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
 * Verifies the state machine behaviour upon encountering an emergency in PreReady.
 */
TEST_F(DemoRunTest, demoPreReadyEmergency)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
    testCalibratingToPreReady();
    testPreReadyEmergency();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Ready.
 */
TEST_F(DemoRunTest, demoReadyEmergency)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
    testCalibratingToPreReady();
    testPreReadyToReady();
    testReadyEmergency();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Accelerating.
 */
TEST_F(DemoRunTest, demoAcceleratingEmergency)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
    testCalibratingToPreReady();
    testPreReadyToReady();
    testReadyToAccelerating();
    testAcceleratingEmergency();
    testFailurePreBrakingToFailureBraking();
    testFailureBrakingToStopped();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in Cruising.
 */
TEST_F(DemoRunTest, demoCruisingEmergency)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
    testCalibratingToPreReady();
    testPreReadyToReady();
    testReadyToAccelerating();
    testAcceleratingToCruising();
    testCruisingEmergency();
    testFailurePreBrakingToFailureBraking();
    testFailureBrakingToStopped();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in PreBraking without
 * the Cruising state
 */
TEST_F(DemoRunTest, demoPreBrakingWithoutCruisingEmergency)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
    testCalibratingToPreReady();
    testPreReadyToReady();
    testReadyToAccelerating();
    testAcceleratingToPreBraking();
    testPreBrakingEmergency();
    testFailurePreBrakingToFailureBraking();
    testFailureBrakingToStopped();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

/**
 * Verifies the state machine behaviour upon encountering an emergency in PreBraking with
 * the Cruising state
 */
TEST_F(DemoRunTest, demoPreBrakingWithCruisingEmergency)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
    testCalibratingToPreReady();
    testPreReadyToReady();
    testReadyToAccelerating();
    testAcceleratingToCruising();
    testCruisingToPreBraking();
    testPreBrakingEmergency();
    testFailurePreBrakingToFailureBraking();
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
TEST_F(DemoRunTest, demoBrakingEmergencyWithoutCruising)
{
  for (int i = 0; i < kTestSize; i++) {
    utils::System &sys = utils::System::getSystem();
    sys.parseArgs(2, kDefaultArgs);
    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
    testCalibratingToPreReady();
    testPreReadyToReady();
    testReadyToAccelerating();
    testAcceleratingToPreBraking();
    testPreBrakingToNominalBraking();
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
TEST_F(DemoRunTest, demoBrakingEmergencyWithCruising)
{
  for (std::size_t i = 0; i < kTestSize; ++i) {
    utils::System::parseArgs(2, kDefaultArgs);

    initialiseData();

    utils::concurrent::Thread *state_machine = new demo_state_machine::Main();
    state_machine->start();

    waitForUpdate();

    testIdleToPreCalibrating();
    testPreCalibratingToCalibrating();
    testCalibratingToPreReady();
    testPreReadyToReady();
    testReadyToAccelerating();
    testAcceleratingToCruising();
    testCruisingToPreBraking();
    testPreBrakingToNominalBraking();
    testNominalBrakingEmergency();
    testFailureBrakingToStopped();
    testFailureStoppedToOff();

    state_machine->join();
    delete state_machine;
  }
}

}  // namespace hyped::testing
