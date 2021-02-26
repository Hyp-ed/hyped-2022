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

#include <stdlib.h>

#include <string>
#include <vector>

#include "data/data.hpp"
#include "gtest/gtest.h"
#include "state_machine/state.hpp"
#include "state_machine/transitions.hpp"
#include "utils/logger.hpp"

using namespace hyped::data;
using namespace hyped::state_machine;

class Randomiser {
  //---------------------------------------------------------------------------
  // Global Module States
  //---------------------------------------------------------------------------

  void randomiseModuleStatus(ModuleStatus &module_status) 
  {
    // Randomises the module status.
    constexpr int num_statuses = 4;
    module_status = static_cast<ModuleStatus>(rand() % num_statuses);
  }

  void randomiseNavigation(Navigation &nav_data) 
  {
    // Generates a run length between 750 and 1749. 
    // Initially declared run length = 1250
    nav_data.run_length = static_cast<Navigation>(rand() % 1000 + 750);

    // Generates a braking buffer length between 10 and 29. 
    // Initially declared braking buffer length = 20
    nav_data.braking_buffer = static_cast<Navigation>(rand() % 20 + 10);

    // Generates a displacement length between 750 and 1749. 
    nav_data.displacement = static_cast<Navigation>(rand() % 1000 + 750);

    // Generates a velocity length between 75 and 174. 
    nav_data.velocity = static_cast<Navigation>(rand() % 100 + 75);  

    // Generates an acceleration length between 75 and 174. 
    nav_data.acceleration = static_cast<Navigation>(rand () % 100 + 75);  

    // Generates an emergency braking distance length between 600 and 899. 
    nav_data.emergency_braking_distance = static_cast<Navigation>(rand() % 300 + 600);

    // Generates a braking distance length between 600 and 899. 
    // Initially declared braking distance = 750
    nav_data.braking_distance = static_cast<Navigation>(rand() % 300 + 600); 
  }

  //---------------------------------------------------------------------------
  // Raw Sensor data
  //---------------------------------------------------------------------------

  void randomiseOperationalData(Sensor &operational) 
  {
    // Generates a random bool value for operational.
    operational = static_cast<ImuData>(rand() < (RAND_MAX / 2));
  }
  
  void randomiseImuData(ImuData &imu_data) 
  {
    for (int i = 0; i < 3; i++) {
      imu_data.acc[i] = static_cast<ImuData>(rand() % 100 + 75);
      imu_data.fifo[i] = static_cast<ImuData>(rand() % 100 + 75);
    }  
  }

  void randomiseEncoderData(EncoderData &encoder_data) 
  {
    // Generates a disp value between 750 and 1749 in accord to the randomised displacement value.
    encoder_data.disp = static_cast<EncoderData>(rand() % 1000 + 750);
  }

  void randomiseStripeCounter(StripeCounter &stripe_counter) 
  {
    // Generates a count value between 0 and 10.
    stripe_counter.count = static_cast<StripeCounter>(rand() % 11);
  }

  void randomiseTemperatureData(TemperatureData &temp_data) 
  {
    // Generates a temperature value between 0 and 99 C.
    temp_data.temp = static_cast<TemperatureData>(rand() % 100);
  }

  void randomiseSensorsData(Sensors &sensors_data) 
  {
    // Not completed yet
    sensors_data.imu = static_cast<Sensors>();
    sensors_data.encoder = static_cast<Sensors>();
    sensors_data.keyence_stripe_counter = static_cast<Sensors>();
  }

  void randomiseBatteryData(BatteryData &battery_data) 
  {
    // Generates a voltage data between 0 and 499.
    battery_data.voltage = static_cast<BatteryData>(rand() % 500);

    // Generates a current data between 0 and 499.
    battery_data.current = static_cast<BatteryData>(rand() % 500);

    // Generates a charge percentage data between 0 and 100.
    battery_data.charge = static_cast<BatteryData>(rand() % 101);

    // Generates an average temperature data between 0 and 99 C.
    battery_data.average_temperature = static_cast<BatteryData>(rand % 100);

    // Below only for BMSHP! Value for BMSLP = 0
    // Generates a cell voltage data between 0 and 50000 mV.
    battery_data.cell_voltage = static_cast<BatteryData>((rand() % 500) * 100);

    // Generates a low temperature data between 0 and 99 C.
    battery_data.low_temperature = static_cast<BatteryData>(rand % 100);

    // Generates a high temperature data between 0 and 99 C.
    battery_data.high_temperature = static_cast<BatteryData>(rand % 100);
    
    // Generates a low cell voltage data between 0 and 50000 mV.
    battery_data.low_voltage_cell = static_cast<BatteryData>((rand() % 500) * 100);

    // Generates a high cell voltage data between 0 and 50000 mV.
    battery_data.high_voltage_cell = static_cast<BatteryData>((rand() % 500) * 100);

    // Generates a random bool value for IMD fault.
    battery_data.imd_fault = static_cast<BatteryData>(rand() < (RAND_MAX / 2));
  }

  void randomiseBatteries(Batteries &batteries_data) 
  {
    // Not completed yet
    batteries_data.low_power_batteries = static_cast<Batteries>();
    batteries_data.high_power_batteries = static_cast<Batteries>();
  }

  void randomiseEmbrakes(EmergencyBrakes &embrakes_data) 
  {
    // Generates a random bool value for brakes_retracted.
    embrakes_data.brakes_retracted = static_cast<EmergencyBrakes>(rand() < (RAND_MAX / 2));
  }

  //---------------------------------------------------------------------------
  // Motor data
  //---------------------------------------------------------------------------

  void randomiseMotors(Motors &motors_data) 
  {
    // Generates a RPM data between 0 and 199 for all 4 motors.
    for (int i = 0; i < motors_data.kNumMotors; i++) {
      motors_data.rpms[i] = static_cast<Motors>(rand() % 200);
    }
  }

  //---------------------------------------------------------------------------
  // Telemetry data
  //---------------------------------------------------------------------------

  void randomiseTelemetry(Telemetry &telemetry_data) 
  {
    // Generates a random bool value for all telemetry commands.
    telemetry_data.calibrate_command = static_cast<Telemetry>(rand() < (RAND_MAX / 2));
    telemetry_data.launch_command = static_cast<Telemetry>(rand() < (RAND_MAX / 2));
    telemetry_data.shutdown_command = static_cast<Telemetry>(rand() < (RAND_MAX / 2));
    telemetry_data.service_propulsion_go = static_cast<Telemetry>(rand() < (RAND_MAX / 2));
    telemetry_data.emergency_stop_command = static_cast<Telemetry>(rand() < (RAND_MAX / 2));
    telemetry_data.nominal_braking_command = static_cast<Telemetry>(rand() < (RAND_MAX / 2));
  }

  //---------------------------------------------------------------------------
  // State Machine States
  //---------------------------------------------------------------------------

  void randomiseStateMachine(StateMachine &stm_data) 
  {
    // Not completed yet
    constexpr int num_states = 11;
  }
};
