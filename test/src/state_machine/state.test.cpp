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

  void randomiseModuleStatus(ModuleStatus &module_status) {}

  //---------------------------------------------------------------------------
  // Navigation
  //---------------------------------------------------------------------------

  void randomiseNavigation(Navigation &nav_data) {}

  //---------------------------------------------------------------------------
  // Raw Sensor data
  //---------------------------------------------------------------------------

  void randomiseImuData(ImuData &imu_data) {}

  void randomiseEncoderData(EncoderData &encoder_data) {}

  void randomiseStripeCounter(StripeCounter &stripe_counter) {}

  void randomiseTemperatureData(TemperatureData &temp_data) {}

  void randomiseSensorsData(Sensors &sensors_data) {}

  void randomiseBatteryData(BatteryData &battery_data) {}

  void randomiseBatteries(Batteries &batteries_data) {}

  void randomiseEmbrakes(EmergencyBrakes &embrakes_data) {}

  //---------------------------------------------------------------------------
  // Motor data
  //---------------------------------------------------------------------------

  void randomiseMotors(Motors &motors_data) {}

  //---------------------------------------------------------------------------
  // Telemetry data
  //---------------------------------------------------------------------------

  void randomiseTelemetry(Telemetry &telemetry_data) {}

  //---------------------------------------------------------------------------
  // State Machine States
  //---------------------------------------------------------------------------

  void randomiseStateMachine(StateMachine &stm_data) {}
};
