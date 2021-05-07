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
  static constexpr int TEST_SIZE = 1;

  // ---- Logger ---------------

  hyped::utils::Logger log;
  hyped::utils::Logger stm_log;
  int stdout_f;
  int tmp_stdout_f;

  // ---- Error messages -------

  const std::string encountered_critical_failure_error = "We encountered critical failure.";
  const std::string not_encountered_critical_failure_error =
                                                         "Did not encounter critical failure.";
  const std::string transition_failed_error = "Did not transition";

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

  void waitForUpdate() { Thread::sleep(100); }

  /**
   *  This array is used for randomising which module status should be set to
   *  kCriticalFailure in order to create the emergency scenario.
   */
  std::array<ModuleStatus, 6> module_statuses = { embrakes_data.module_status,
                                                  nav_data.module_status,
                                                  telemetry_data.module_status,
                                                  motors_data.module_status,
                                                  sensors_data.module_status,
                                                  batteries_data.module_status };

  // ---- Run steps --------------


  void initialiseData()
  {
    randomiseInternally();

    embrakes_data.module_status    = ModuleStatus::kStart;
    nav_data.module_status         = ModuleStatus::kStart;
    telemetry_data.module_status   = ModuleStatus::kStart;
    motors_data.module_status      = ModuleStatus::kStart;
    sensors_data.module_status     = ModuleStatus::kStart;
    batteries_data.module_status   = ModuleStatus::kStart;

    writeData();
  }


  void checkIdleToCalibrating()
  {
    randomiseInternally();

    telemetry_data.calibrate_command = true;
    embrakes_data.module_status      = ModuleStatus::kInit;
    nav_data.module_status           = ModuleStatus::kInit;
    telemetry_data.module_status     = ModuleStatus::kInit;
    motors_data.module_status        = ModuleStatus::kInit;
    sensors_data.module_status       = ModuleStatus::kInit;
    batteries_data.module_status     = ModuleStatus::kInit;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.critical_failure, false) << encountered_critical_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kCalibrating) <<
                                                              transition_failed_error;
  }


  void checkIdleEmergency()
  {
    randomiseInternally();

    int random = static_cast<int8_t>(rand() % 6);

    module_statuses[random] = ModuleStatus::kCriticalFailure;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.critical_failure, true) << not_encountered_critical_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFailureStopped) <<
                                                                 transition_failed_error;
  }


  void checkCalibratingToReady()
  {
    randomiseInternally();

    embrakes_data.module_status = ModuleStatus::kReady;
    nav_data.module_status      = ModuleStatus::kReady;
    motors_data.module_status   = ModuleStatus::kReady;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.critical_failure, false) << encountered_critical_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kReady) <<
                                                              transition_failed_error;
  }


  void checkCalibratingEmergency()
  {
    randomiseInternally();

    int random = static_cast<int8_t>(rand() % 6);

    module_statuses[random] = ModuleStatus::kCriticalFailure;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.critical_failure, true) << not_encountered_critical_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFailureStopped) <<
                                                                 transition_failed_error;
  }

  void checkReadyToAccelerating()
  {
    randomiseInternally();

    initialiseData();

    telemetry_data.launch_command = true;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.critical_failure, false) << encountered_critical_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kAccelerating) <<
                                                              transition_failed_error;
  }

  void checkReadyEmergency()
  {
    randomiseInternally();

    int random = static_cast<int8_t>(rand() % 6);

    module_statuses[random] = ModuleStatus::kCriticalFailure;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.critical_failure, true) << not_encountered_critical_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFailureStopped) <<
                                                                 transition_failed_error;
  }

  void checkAcceleratingToNominalBraking()
  {
    randomiseInternally();

    // These values are assigned to ensure the transition conditions.
    nav_data.displacement = 1000;
    nav_data.braking_distance = 1000;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.critical_failure, false) << encountered_critical_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kNominalBraking) <<
                                                              transition_failed_error;
  }

  void checkAcceleratingToCruising()
  {
    // Not completed yet
  }

  void checkCruisingEmergency()
  {
    // Not completed yet
  }

  void checkAcceleratingEmergency()
  {
    randomiseInternally();

    int random = static_cast<int8_t>(rand() % 6);

    module_statuses[random] = ModuleStatus::kCriticalFailure;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.critical_failure, true) << not_encountered_critical_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kEmergencyBraking) <<
                                                                 transition_failed_error;
  }

  void checkNominalBrakingToFinished()
  {
    randomiseInternally();

    nav_data.velocity = 0;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.critical_failure, false) << encountered_critical_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFinished) <<
                                                              transition_failed_error;
  }

  void checkNominalBrakingEmergency()
  {
    randomiseInternally();

    int random = static_cast<int8_t>(rand() % 6);

    module_statuses[random] = ModuleStatus::kCriticalFailure;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.critical_failure, true) << not_encountered_critical_failure_error;
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kEmergencyBraking) <<
                                                                 transition_failed_error;
  }

  void checkFinishedToOff()
  {
    randomiseInternally();

    telemetry_data.shutdown_command = true;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.current_state, hyped::data::State::kExiting) << transition_failed_error;
  }

  void checkFailureBrakingToStopped()
  {
    randomiseInternally();

    nav_data.velocity = 0;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.current_state, hyped::data::State::kFailureStopped) <<
                                                         transition_failed_error;
  }

  void checkFailureStoppedToOff()
  {
    randomiseInternally();

    telemetry_data.shutdown_command = true;

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.current_state, hyped::data::State::kExiting) << transition_failed_error;
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

/*******************************

TEST_F(RunTest, fullRun)
{
  System &sys = System::getSystem();
  initialiseData();
  Thread *state_machine = new Main(0, stm_log);
  state_machine->start();

  for (int i = 0; i < TEST_SIZE; i++) {
    checkIdleToCalibrating();
    checkCalibratingToReady();
    checkReadyToAccelerating();
    checkAcceleratingToNominalBraking();
    checkNominalBrakingToFinished();
    checkFinishedToOff();
  }
  sys.running_ = false;
  state_machine->join();
}

**********************************/
