/*
 * Author: Franz Miltz
 * Organisation: HYPED
 * Date:
 * Description:
 *
 *    Copyright 2019 HYPED
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
#include "state_machine/state.hpp"
#include "state_machine/transitions.hpp"
#include "utils/logger.hpp"

using namespace hyped::data;
using namespace hyped::state_machine;

struct StateTest : public ::testing::Test {
  // ---- Logger ---------------

  hyped::utils::Logger log;
  int stdout_f;
  int tmp_stdout_f;

  // ---- Test size -----------

  static constexpr int TEST_SIZE = 1000;

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
// Test structures
//---------------------------------------------------------------------------

struct IdleTest : public StateTest { };

struct CalibratingTest : public StateTest { };

struct ReadyTest : public StateTest { };

struct AcceleratingTest : public StateTest { };

struct NominalBrakingTest : public StateTest { };

struct FinishedTest : public StateTest { };

struct FailureBrakingTest : public StateTest { };

struct FailureStoppedTest : public StateTest { };

//---------------------------------------------------------------------------
// Randomiser
//---------------------------------------------------------------------------

class Randomiser {
 public:
  Randomiser()
  {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<nav_t> distribution(0.0, 1.0);
  }

  static nav_t randomDecimal()
  {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<nav_t> distribution(0.0, 1.0);
    return distribution(generator);
  }

  //---------------------------------------------------------------------------
  // Global Module States
  //---------------------------------------------------------------------------

  static void randomiseModuleStatus(ModuleStatus &module_status)
  {
    // Randomises the module status.
    constexpr int num_statuses = 4;
    module_status = static_cast<ModuleStatus>(rand() % num_statuses);
  }

  static void randomiseNavigation(Navigation &nav_data)
  {
    // Generates a displacement length between 750 and 1749.
    nav_data.displacement = static_cast<nav_t>((rand() % 1000 + 750) + randomDecimal());

    // Generates a velocity length between -10 and 200.
    nav_data.velocity = static_cast<nav_t>((rand() % 201 - 10) + randomDecimal());

    // Generates an acceleration length between 75 and 174.
    nav_data.acceleration = static_cast<nav_t>((rand() % 100 + 75) + randomDecimal());

    // Generates an emergency braking distance length between 600 and 899.
    nav_data.emergency_braking_distance =
      static_cast<nav_t>((rand() % 300 + 600) + randomDecimal());

    // Generates a braking distance length between 600 and 899.
    // Initially declared braking distance = 750
    nav_data.braking_distance = static_cast<nav_t>((rand() % 300 + 600) + randomDecimal());
  }

  //---------------------------------------------------------------------------
  // Raw Sensor data
  //---------------------------------------------------------------------------

  static void randomiseImuData(ImuData &imu_data)
  {
    for (int i = 0; i < 3; i++) {
      imu_data.acc[i] = static_cast<nav_t>((rand() % 100 + 75) + randomDecimal());
    }
    for (int i = 0; i < 3; i++) {
      imu_data.fifo.at(i) = static_cast<NavigationVector>((rand() % 100 + 75) + randomDecimal());
    }
  }

  static void randomiseEncoderData(EncoderData &encoder_data)
  {
    // Generates a disp value between 750 and 1749 in accord to the randomised displacement value.
    encoder_data.disp = static_cast<nav_t>((rand() % 1000 + 750) + randomDecimal());
  }

  static void randomiseStripeCounter(StripeCounter &stripe_counter)
  {
    // Generates a count timestamp and value between 0 and 10.
    stripe_counter.count.timestamp = static_cast<uint32_t>(rand() % 11);
    stripe_counter.count.value = static_cast<uint32_t>(rand() % 11);
  }

  static void randomiseTemperatureData(TemperatureData &temp_data)
  {
    // Generates a temperature value between 0 and 99 C.
    temp_data.temp = static_cast<int>(rand() % 100);
  }

  static void randomiseBatteryData(BatteryData &battery_data)
  {
    // Generates a voltage data between 0 and 499.
    battery_data.voltage = static_cast<uint16_t>(rand() % 500);

    // Generates a current data between 0 and 499.
    battery_data.current = static_cast<int16_t>(rand() % 500);

    // Generates a charge percentage data between 0 and 100.
    battery_data.charge = static_cast<uint8_t>(rand() % 101);

    // Generates an average temperature data between 0 and 99 C.
    battery_data.average_temperature = static_cast<int8_t>(rand() % 100);

    // Below only for BMSHP! Value for BMSLP = 0
    // Generates a cell voltage data between 0 and 50000 mV.
    for (int i = 0; i < 36; i++) {
      battery_data.cell_voltage[i] = static_cast<uint16_t>((rand() % 500) * 100);
    }

    // Generates a low temperature data between 0 and 99 C.
    battery_data.low_temperature = static_cast<int8_t>(rand() % 100);

    // Generates a high temperature data between 0 and 99 C.
    battery_data.high_temperature = static_cast<int8_t>(rand() % 100);

    // Generates a low cell voltage data between 0 and 50000 mV.
    battery_data.low_voltage_cell = static_cast<uint16_t>((rand() % 500) * 100);

    // Generates a high cell voltage data between 0 and 50000 mV.
    battery_data.high_voltage_cell = static_cast<uint16_t>((rand() % 500) * 100);

    // Generates a random bool value for IMD fault.
    battery_data.imd_fault = static_cast<bool>(rand() > (RAND_MAX / 2));
  }

  static void randomiseEmbrakes(EmergencyBrakes &embrakes_data)
  {
    for (int i = 0; i < embrakes_data.kNumEmbrakes; i++) {
      embrakes_data.brakes_retracted[i] = static_cast<bool>(rand() > (RAND_MAX / 2));
    }
  }

  //---------------------------------------------------------------------------
  // Motor data
  //---------------------------------------------------------------------------

  static void randomiseMotors(Motors &motors_data)
  {
    // Generates a RPM data between 0 and 199 for all 4 motors.
    for (int i = 0; i < motors_data.kNumMotors; i++) {
      motors_data.rpms[i] = static_cast<uint32_t>(rand() % 200);
    }
  }

  //---------------------------------------------------------------------------
  // Telemetry data
  //---------------------------------------------------------------------------

  static void randomiseTelemetry(Telemetry &telemetry_data)
  {
    // Generates a random bool value for all telemetry commands.
    telemetry_data.calibrate_command = static_cast<bool>(rand() > (RAND_MAX / 2));
    telemetry_data.launch_command = static_cast<bool>(rand() > (RAND_MAX / 2));
    telemetry_data.shutdown_command = static_cast<bool>(rand() > (RAND_MAX / 2));
    telemetry_data.service_propulsion_go = static_cast<bool>(rand() > (RAND_MAX / 2));
    telemetry_data.emergency_stop_command = static_cast<bool>(rand() > (RAND_MAX / 2));
    telemetry_data.nominal_braking_command = static_cast<bool>(rand() > (RAND_MAX / 2));
  }

  //---------------------------------------------------------------------------
  // State Machine States
  //---------------------------------------------------------------------------

  static void randomiseStateMachine(StateMachine &stm_data)
  {
    stm_data.critical_failure = static_cast<bool>(rand() > (RAND_MAX / 2));
  }

  static void generateAllPermutations(ModuleStatus &module_status, EmergencyBrakes &embrakes_data,
                            Navigation &nav_data, Batteries &batteries_data,
                            Telemetry &telemetry_data, Sensors &sensors_data, Motors &motors_data)
  {
    constexpr int num_statuses = 4;
    constexpr int num_modules = 6;
    for (int i = 0; i <= pow(static_cast<double>(num_statuses),
      static_cast<double>(num_modules)); i++)
      {
      embrakes_data.module_status = static_cast<ModuleStatus>(i % num_modules);
      i /= num_statuses;
      nav_data.module_status = static_cast<ModuleStatus>(i % num_modules);
      i /= num_statuses;
      batteries_data.module_status = static_cast<ModuleStatus>(i % num_modules);
      i /= num_statuses;
      telemetry_data.module_status = static_cast<ModuleStatus>(i % num_modules);
      i /= num_statuses;
      sensors_data.module_status = static_cast<ModuleStatus>(i % num_modules);
      i /= num_statuses;
      motors_data.module_status = static_cast<ModuleStatus>(i % num_modules);
      i /= num_statuses;
    }
  }
};


//---------------------------------------------------------------------------
//--------------------------------- TESTS -----------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Idle Tests
//---------------------------------------------------------------------------

TEST_F(IdleTest, handlesEmergency)
{
  Data &data = Data::getInstance();
  Idle *state = Idle::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                      telemetry_data, sensors_data, motors_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (has_emergency) {
      ASSERT_EQ(new_state, FailureStopped::getInstance());
    } else {
      ASSERT_NE(new_state, FailureStopped::getInstance());
    }
  }
}

TEST_F(IdleTest, handlesCalibrateCommand)
{
  Data &data = Data::getInstance();
  Idle *state = Idle::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                      telemetry_data, sensors_data, motors_data);

    if (!has_emergency) {
      bool calibrate_command = checkCalibrateCommand(log, telemetry_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (!calibrate_command) {
        ASSERT_EQ(new_state, nullptr);
      }
    }
  }
}

TEST_F(IdleTest, handlesAllInitialised)
{
  Data &data = Data::getInstance();
  Idle *state = Idle::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                      telemetry_data, sensors_data, motors_data);

    bool calibrate_command = checkCalibrateCommand(log, telemetry_data);

    if (!has_emergency && !calibrate_command) {
      bool all_initialised = checkModulesInitialised(log, embrakes_data, nav_data, batteries_data,
                                                 telemetry_data, sensors_data, motors_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (all_initialised) {
        ASSERT_EQ(new_state, FailureStopped::getInstance());
      } else {
        ASSERT_NE(new_state, FailureStopped::getInstance());
      }
    }
  }
}

//---------------------------------------------------------------------------
// Calibrating Tests
//---------------------------------------------------------------------------

TEST_F(CalibratingTest, handlesEmergency)
{
  Data &data = Data::getInstance();
  Calibrating *state = Calibrating::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                      telemetry_data, sensors_data, motors_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (has_emergency) {
      ASSERT_EQ(new_state, FailureStopped::getInstance());
    } else {
      ASSERT_NE(new_state, FailureStopped::getInstance());
    }
  }
}

TEST_F(CalibratingTest, handlesAllReady)
{
  Data &data = Data::getInstance();
  Calibrating *state = Calibrating::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                      telemetry_data, sensors_data, motors_data);

    if (!has_emergency) {
      bool all_ready = checkModulesReady(log, embrakes_data, nav_data, motors_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (all_ready) {
        ASSERT_EQ(new_state, Ready::getInstance());
      } else {
        ASSERT_NE(new_state, Ready::getInstance());
      }
    }
  }
}

//---------------------------------------------------------------------------
// Ready Tests
//---------------------------------------------------------------------------

TEST_F(ReadyTest, handlesEmergency)
{
  Data &data = Data::getInstance();
  Ready *state = Ready::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                      telemetry_data, sensors_data, motors_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (has_emergency) {
      ASSERT_EQ(new_state, FailureStopped::getInstance());
    } else {
      ASSERT_NE(new_state, FailureStopped::getInstance());
    }
  }
}

TEST_F(ReadyTest, handlesLaunchCommand)
{
  Data &data = Data::getInstance();
  Ready *state = Ready::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                      telemetry_data, sensors_data, motors_data);

    if (!has_emergency) {
      bool received_launch_command = checkLaunchCommand(log, telemetry_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (received_launch_command) {
        ASSERT_EQ(new_state, Accelerating::getInstance());
      } else {
        ASSERT_NE(new_state, Accelerating::getInstance());
      }
    }
  }
}

//---------------------------------------------------------------------------
// Accelerating Tests
//---------------------------------------------------------------------------

TEST_F(AcceleratingTest, handlesEmergency)
{
  Data &data = Data::getInstance();
  Accelerating *state = Accelerating::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                      telemetry_data, sensors_data, motors_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (has_emergency) {
      ASSERT_EQ(new_state, FailureBraking::getInstance());
    } else {
      ASSERT_NE(new_state, FailureBraking::getInstance());
    }
  }
}

TEST_F(AcceleratingTest, handlesInBrakingZone)
{
  Data &data = Data::getInstance();
  Accelerating *state = Accelerating::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                      telemetry_data, sensors_data, motors_data);

    if (!has_emergency) {
      bool in_braking_zone = checkEnteredBrakingZone(log, nav_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (in_braking_zone) {
        ASSERT_EQ(new_state, NominalBraking::getInstance());
      } else {
        ASSERT_NE(new_state, NominalBraking::getInstance());
      }
    }
  }
}

//---------------------------------------------------------------------------
// Nominal Braking Tests
//---------------------------------------------------------------------------

TEST_F(NominalBrakingTest, handlesEmergency)
{
  Data &data = Data::getInstance();
  NominalBraking *state = NominalBraking::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                      telemetry_data, sensors_data, motors_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (has_emergency) {
      ASSERT_EQ(new_state, FailureBraking::getInstance());
    } else {
      ASSERT_NE(new_state, FailureBraking::getInstance());
    }
  }
}

TEST_F(NominalBrakingTest, handlesStopped)
{
  Data &data = Data::getInstance();
  NominalBraking *state = NominalBraking::getInstance();

  EmergencyBrakes embrakes_data;
  Navigation nav_data;
  Batteries batteries_data;
  Telemetry telemetry_data;
  Sensors sensors_data;
  Motors motors_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseEmbrakes(embrakes_data);
    Randomiser::randomiseNavigation(nav_data);
    Randomiser::randomiseTelemetry(telemetry_data);
    Randomiser::randomiseMotors(motors_data);

    data.setEmergencyBrakesData(embrakes_data);
    data.setNavigationData(nav_data);
    data.setTelemetryData(telemetry_data);
    data.setMotorData(motors_data);

    bool has_emergency = checkEmergency(log, embrakes_data, nav_data, batteries_data,
                                      telemetry_data, sensors_data, motors_data);

    if (!has_emergency) {
      bool stopped = checkPodStopped(log, nav_data);
      hyped::state_machine::State *new_state = state->checkTransition(log);

      if (stopped) {
        ASSERT_EQ(new_state, Finished::getInstance());
      }
    }
  }
}

//---------------------------------------------------------------------------
// Finished Tests
//---------------------------------------------------------------------------

TEST_F(FinishedTest, handlesShutdownCommand)
{
  Data &data = Data::getInstance();
  Finished *state = Finished::getInstance();

  Telemetry telemetry_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseTelemetry(telemetry_data);

    data.setTelemetryData(telemetry_data);

    bool received_shutdown_command = checkShutdownCommand(log, telemetry_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (received_shutdown_command) {
      ASSERT_EQ(new_state, Off::getInstance());
    } else {
      ASSERT_NE(new_state, Off::getInstance());
    }
  }
}

//---------------------------------------------------------------------------
// Failure Braking Tests
//---------------------------------------------------------------------------

TEST_F(FailureBrakingTest, handlesStopped)
{
  Data &data = Data::getInstance();
  FailureBraking *state = FailureBraking::getInstance();

  Navigation nav_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseNavigation(nav_data);

    data.setNavigationData(nav_data);

    bool stopped = checkPodStopped(log, nav_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (stopped) {
      ASSERT_EQ(new_state, FailureStopped::getInstance());
    }
  }
}

//---------------------------------------------------------------------------
// Failure Stopped Tests
//---------------------------------------------------------------------------

TEST_F(FailureStoppedTest, handlesShutdownCommand)
{
  Data &data = Data::getInstance();
  FailureStopped *state = FailureStopped::getInstance();

  Telemetry telemetry_data;

  for (int i = 0; i < TEST_SIZE; i++) {
    Randomiser::randomiseTelemetry(telemetry_data);

    data.setTelemetryData(telemetry_data);

    bool received_shutdown_command = checkShutdownCommand(log, telemetry_data);
    hyped::state_machine::State *new_state = state->checkTransition(log);

    if (received_shutdown_command) {
      ASSERT_EQ(new_state, Off::getInstance());
    } else {
      ASSERT_NE(new_state, Off::getInstance());
    }
  }
}
