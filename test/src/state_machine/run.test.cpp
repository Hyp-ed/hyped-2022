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

  // TODO(efe-ozbatur)

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

  void randomiseInternally()
  {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);
    Randomiser::randomiseSensorsData(sensors_data);
    Randomiser::randomiseBatteriesData(batteries_data);
  }

  void writeData()
  {
    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);
    data.setSensorsData(sensors_data);
    data.setBatteriesData(batteries_data);
  }

  void readData()
  {
    embrakes_data = data.getEmergencyBrakesData();
    stm_data      = data.getStateMachineData();
    // ...
  }

  void waitForUpdate() { Thread::sleep(100); }

  // ---- Run steps --------------

  void initialiseData()
  {
    randomiseInternally();

    embrakes_data.module_status = ModuleStatus::kStart;
    // ...

    writeData();
  }

  void checkIdleToCalibrating()
  {
    randomiseInternally();

    telemetry_data.calibrate_command = true;
    embrakes_data.module_status      = ModuleStatus::kInit;
    // ...

    writeData();

    waitForUpdate();

    readData();

    ASSERT_EQ(stm_data.critical_failure, false) << "We encountered critical failure.";
    ASSERT_EQ(stm_data.current_state, hyped::data::State::kCalibrating) << "Did not transition.";
  }

  void checkIdleEmergency() {}

  void checkCalibratingToReady() {}

  void checkCalibratingEmergency() {}

  void checkReadyToAccelerating() {}

  void checkReadyEmergency() {}

  void checkAcceleratingToNominalBraking() {}

  void checkAcceleratingToCruising() {}

  void checkCruisingEmergency() {}

  void checkAcceleratingEmergency() {}

  void checkNominalBrakingToFinished() {}

  void checkNominalBrakingEmergency() {}

  void checkFinishedToOff() {}

  void checkFailureBrakingToStopped() {}

  void checkFailureStoppedToOff() {}

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
