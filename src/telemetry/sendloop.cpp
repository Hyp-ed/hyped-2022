/*
 * Author: Neil Weidinger
 * Organisation: HYPED
 * Date: April 2019
 * Description:
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "telemetry/sendloop.hpp"
#include "telemetry/utils.hpp"

namespace hyped {
namespace telemetry {

SendLoop::SendLoop(Logger& log, data::Data& data, Main* main_pointer)
  : Thread {log},
    main_ref_ {*main_pointer},
    data_ {data}
{
  log_.DBG("Telemetry", "Telemetry SendLoop thread object created");
}

void SendLoop::run()
{
  log_.DBG("Telemetry", "Telemetry SendLoop thread started");

  telemetry_data::ClientToServer msg;

  while (true) {
    packNavigationMessage(msg);
    packStateMachineMessage(msg);
    packMotorsMessage(msg);
    packBatteriesMessage(msg);
    packSensorsMessage(msg);
    packTemperatureMessage(msg);
    packEmergencyBrakesMessage(msg);

    try {
      main_ref_.client_.sendData(msg);
    }
    catch (std::exception& err) {
      log_.ERR("Telemetry", "%s", err.what());

      data::Telemetry telem_data_struct = data_.getTelemetryData();
      telem_data_struct.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telem_data_struct);

      break;
    }

    msg.Clear();

    Thread::sleep(100);
  }

  log_.DBG("Telemetry", "Exiting Telemetry SendLoop thread");
}

void SendLoop::packNavigationMessage(telemetry_data::ClientToServer& msg)
{
  data::Navigation nav_data = data_.getNavigationData();
  telemetry_data::ClientToServer::Navigation* navigation_msg = msg.mutable_navigation();

  navigation_msg->set_module_status(Utils::moduleStatusEnumConversion(nav_data.module_status));
  navigation_msg->set_distance(nav_data.distance);
  navigation_msg->set_velocity(nav_data.velocity);
  navigation_msg->set_acceleration(nav_data.acceleration);
}

void SendLoop::packStateMachineMessage(telemetry_data::ClientToServer& msg)
{
  data::StateMachine sm_data = data_.getStateMachineData();
  telemetry_data::ClientToServer::StateMachine* state_machine_msg = msg.mutable_state_machine();

  state_machine_msg->set_current_state(Utils::stateEnumConversion(sm_data.current_state));
}

void SendLoop::packMotorsMessage(telemetry_data::ClientToServer& msg)
{
  data::Motors motor_data = data_.getMotorData();
  telemetry_data::ClientToServer::Motors* motors_msg = msg.mutable_motors();

  motors_msg->set_module_status(Utils::moduleStatusEnumConversion(motor_data.module_status));
}

void SendLoop::packBatteriesMessage(telemetry_data::ClientToServer& msg)
{
  data::Batteries batteries_data = data_.getBatteriesData();
  telemetry_data::ClientToServer::Batteries* batteries_msg = msg.mutable_batteries();

  batteries_msg->set_module_status(Utils::moduleStatusEnumConversion(batteries_data.module_status)); // NOLINT

  packLpBatteryDataMessage(*batteries_msg, batteries_data.low_power_batteries);
  packHpBatteryDataMessage(*batteries_msg, batteries_data.high_power_batteries);
}

template<std::size_t SIZE>
void SendLoop::packLpBatteryDataMessage(batteriesMsg& batteries_msg, std::array<data::BatteryData, SIZE>& battery_data_array) // NOLINT
{
  for (auto battery_data : battery_data_array) {
    telemetry_data::ClientToServer::Batteries::BatteryData* battery_data_msg = batteries_msg.add_low_power_batteries(); // NOLINT
    packBatteryDataMessageHelper(*battery_data_msg, battery_data);
  }
}

template<std::size_t SIZE>
void SendLoop::packHpBatteryDataMessage(batteriesMsg& batteries_msg, std::array<data::BatteryData, SIZE>& battery_data_array) // NOLINT
{
  for (auto battery_data : battery_data_array) {
    telemetry_data::ClientToServer::Batteries::BatteryData* battery_data_msg = batteries_msg.add_high_power_batteries(); // NOLINT
    packBatteryDataMessageHelper(*battery_data_msg, battery_data);
  }
}

void SendLoop::packBatteryDataMessageHelper(batteriesMsg::BatteryData& battery_data_msg, data::BatteryData& battery_data) // NOLINT
{
  battery_data_msg.set_voltage(battery_data.voltage);
  battery_data_msg.set_current(battery_data.current);
  battery_data_msg.set_charge(battery_data.charge);
  battery_data_msg.set_average_temperature(battery_data.average_temperature);
  battery_data_msg.set_low_temperature(battery_data.low_temperature);
  battery_data_msg.set_high_temperature(battery_data.high_temperature);
  battery_data_msg.set_low_voltage_cell(battery_data.low_voltage_cell);
  battery_data_msg.set_high_voltage_cell(battery_data.high_voltage_cell);

  for (int voltage : battery_data.cell_voltage) {
    battery_data_msg.add_indv_voltage(voltage);
  }
}

void SendLoop::packSensorsMessage(telemetry_data::ClientToServer& msg)
{
  data::Sensors sensors_data = data_.getSensorsData();
  telemetry_data::ClientToServer::Sensors* sensors_msg = msg.mutable_sensors();

  sensors_msg->set_module_status(Utils::moduleStatusEnumConversion(sensors_data.module_status));

  for (data::ImuData imu_data : sensors_data.imu.value) {
    telemetry_data::ClientToServer::Sensors::ImuData* imu_data_msg = sensors_msg->add_imu();

    imu_data_msg->set_operational(imu_data.operational);

    // hardcoded atm to loop three times bc hyped vector doesn't have a method for vector length
    // or have an iterator to support a ranged for loop
    for (int i = 0; i < 3; i++) {
      imu_data_msg->add_acc(imu_data.acc[i]);
    }
  }
}

void SendLoop::packTemperatureMessage(telemetry_data::ClientToServer& msg)
{
  telemetry_data::ClientToServer::Temperature* temp_msg = msg.mutable_temperature();

  temp_msg->set_temperature(data_.getTemperature());
}

void SendLoop::packEmergencyBrakesMessage(telemetry_data::ClientToServer& msg)
{
  data::EmergencyBrakes emergency_brakes_data = data_.getEmergencyBrakesData();
  telemetry_data::ClientToServer::EmergencyBrakes* emergency_brakes_msg = msg.mutable_emergency_brakes(); // NOLINT

  for (bool brake_retracted : emergency_brakes_data.brakes_retracted) {
    emergency_brakes_msg->add_brakes(brake_retracted);
  }
}

}  // namespace telemetry
}  // namespace hyped
