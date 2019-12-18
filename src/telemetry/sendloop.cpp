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

#include <string>
#include "telemetry/sendloop.hpp"

using rapidjson::Writer;
using rapidjson::StringBuffer;

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

  while (true) {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();
    packNavigationMessage(writer);
    packStateMachineMessage(writer);
    packMotorsMessage(writer);
    packBatteriesMessage(writer);
    packSensorsMessage(writer);
    packTemperatureMessage(writer);
    packEmergencyBrakesMessage(writer);
    writer.EndObject();

    if (!main_ref_.client_.sendData(sb.GetString())) {
      log_.ERR("Telemetry", "Error sending message");
      data::Telemetry telem_data_struct = data_.getTelemetryData();
      telem_data_struct.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telem_data_struct);

      break;
    }

    Thread::sleep(100);
  }

  log_.DBG("Telemetry", "Exiting Telemetry SendLoop thread");
}

void SendLoop::packNavigationMessage(Writer<StringBuffer>& writer)
{
  data::Navigation nav_data = data_.getNavigationData();
  writer.Key("navigation");
  writer.StartObject();
  writer.Key("moduleStatus");
  writer.String(convertModuleStatus(nav_data.module_status).c_str());
  writer.Key("distance");
  writer.Double(nav_data.distance);
  writer.Key("velocity");
  writer.Double(nav_data.velocity);
  writer.Key("acceleration");
  writer.Double(nav_data.acceleration);
  writer.EndObject();
}

void SendLoop::packStateMachineMessage(Writer<StringBuffer>& writer)
{
  data::StateMachine sm_data = data_.getStateMachineData();
  writer.Key("stateMachine");
  writer.StartObject();
  writer.Key("currentState");
  writer.String(convertStateMachineState(sm_data.current_state).c_str());
  writer.EndObject();
}

void SendLoop::packMotorsMessage(Writer<StringBuffer>& writer)
{
  data::Motors motor_data = data_.getMotorData();
  writer.Key("motors");
  writer.StartObject();
  writer.Key("moduleStatus");
  writer.String(convertModuleStatus(motor_data.module_status).c_str());
  writer.EndObject();
}

void SendLoop::packBatteriesMessage(Writer<StringBuffer>& writer)
{
  data::Batteries batteries_data = data_.getBatteriesData();
  writer.Key("batteries");
  writer.StartObject();
  writer.Key("moduleStatus");
  writer.String(convertModuleStatus(batteries_data.module_status).c_str());
  writer.Key("lowPowerBatteries");
  packLpBatteryDataMessage(writer, batteries_data.low_power_batteries);
  writer.Key("highPowerBatteries");
  packHpBatteryDataMessage(writer, batteries_data.high_power_batteries);
  writer.EndObject();
}

template<std::size_t SIZE>
void SendLoop::packLpBatteryDataMessage(Writer<StringBuffer>& writer, std::array<data::BatteryData, SIZE>& battery_data_array) // NOLINT
{
  writer.StartArray();
  for (auto battery_data : battery_data_array) {
    packBatteryDataMessageHelper(false, writer, battery_data);
  }
  writer.EndArray();
}

template<std::size_t SIZE>
void SendLoop::packHpBatteryDataMessage(Writer<StringBuffer>& writer, std::array<data::BatteryData, SIZE>& battery_data_array) // NOLINT
{
  writer.StartArray();
  for (auto battery_data : battery_data_array) {
    packBatteryDataMessageHelper(true, writer, battery_data);
  }
  writer.EndArray();
}

void SendLoop::packBatteryDataMessageHelper(bool HP, Writer<StringBuffer>& writer, data::BatteryData& battery_data) // NOLINT
{
  writer.StartObject();
  writer.Key("voltage");
  writer.Int(battery_data.voltage);
  writer.Key("current");
  writer.Int(battery_data.current);
  writer.Key("charge");
  writer.Int(battery_data.charge);
  writer.Key("averageTemperature");
  writer.Int(battery_data.average_temperature);

  if (HP) {
    writer.Key("lowTemperature");
    writer.Int(battery_data.low_temperature);
    writer.Key("highTemperature");
    writer.Int(battery_data.high_temperature);
    writer.Key("lowVoltageCell");
    writer.Int(battery_data.low_voltage_cell);
    writer.Key("highVoltageCell");
    writer.Int(battery_data.high_voltage_cell);
  }

  writer.Key("indvVoltage");
  writer.StartArray();
  for (int voltage : battery_data.cell_voltage) {
    writer.Int(voltage);
  }
  writer.EndArray();
  writer.EndObject();
}

void SendLoop::packSensorsMessage(Writer<StringBuffer>& writer)
{
  data::Sensors sensors_data = data_.getSensorsData();
  writer.Key("sensors");
  writer.StartObject();
  writer.Key("moduleStatus");
  writer.String(convertModuleStatus(sensors_data.module_status).c_str());
  writer.Key("imu");
  writer.StartArray();
  for (data::ImuData imu_data : sensors_data.imu.value) {
    writer.StartObject();
    writer.Key("operational");
    writer.Bool(imu_data.operational);
    writer.Key("acc");
    writer.StartArray();

    // hardcoded atm to loop three times bc hyped vector doesn't have a method for vector length
    // or have an iterator to support a ranged for loop
    for (int i = 0; i < 3; i++) {
      writer.Double(imu_data.acc[i]);
    }
    writer.EndArray();
    writer.EndObject();
  }
  writer.EndArray();
  writer.EndObject();
}

void SendLoop::packTemperatureMessage(Writer<StringBuffer>& writer)
{
  writer.Key("temperature");
  writer.StartObject();
  writer.Key("temperature");
  writer.Int(data_.getTemperature());
  writer.EndObject();
}

void SendLoop::packEmergencyBrakesMessage(Writer<StringBuffer>& writer)
{
  data::EmergencyBrakes emergency_brakes_data = data_.getEmergencyBrakesData();
  writer.Key("emergencyBrakes");
  writer.StartObject();
  writer.Key("brakes");
  writer.StartArray();
  for (bool brake_retracted : emergency_brakes_data.brakes_retracted) {
    writer.Bool(brake_retracted);
  }
  writer.EndArray();
  writer.EndObject();
}

std::string SendLoop::convertStateMachineState(data::State state)
{
  switch (state) {
  case data::State::kInvalid:
    return "INVALID";
    break;
  case data::State::kEmergencyBraking:
    return "EMERGENCY_BRAKING";
    break;
  case data::State::kFailureStopped:
    return "FAILURE_STOPPED";
    break;
  case data::State::kIdle:
    return "IDLE";
    break;
  case data::State::kCalibrating:
    return "CALIBRATING";
    break;
  case data::State::kRunComplete:
    return "RUN_COMPLETE";
    break;
  case data::State::kFinished:
    return "FINISHED";
    break;
  case data::State::kReady:
    return "READY";
    break;
  case data::State::kAccelerating:
    return "ACCELERATING";
    break;
  case data::State::kNominalBraking:
    return "NOMINAL_BRAKING";
    break;
  case data::State::kExiting:
    return "EXITING";
    break;
  default:
    return "";
    break;
  }
}

std::string SendLoop::convertModuleStatus(data::ModuleStatus module_status)
{
  switch (module_status) {
  case data::ModuleStatus::kStart:
    return "START";
    break;
  case data::ModuleStatus::kInit:
    return "INIT";
    break;
  case data::ModuleStatus::kReady:
    return "READY";
    break;
  case data::ModuleStatus::kCriticalFailure:
    return "CRITICAL_FAILURE";
    break;
  default:
    return "";
    break;
  }
}

}  // namespace telemetry
}  // namespace hyped
