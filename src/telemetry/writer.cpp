#include "writer.hpp"

#include <chrono>
#include <cstdint>
#include <string>

#include <data/data.hpp>

namespace hyped::telemetry {

Writer::Writer(data::Data &data) : json_writer_(string_buffer_), data_{data}
{
}

// The current time in milliseconds that will be used later
void Writer::packTime()
{
  json_writer_.Key("time");
  json_writer_.Uint64(std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count());
}

void Writer::packId(const uint16_t id)
{
  json_writer_.Key("id");
  json_writer_.Int(id);
}

void Writer::packTelemetryData()
{
  const auto telemetry_data = data_.getTelemetryData();

  json_writer_.Key("telemetry");
  json_writer_.StartObject();
  // Commands
  json_writer_.Key("calibrate");
  json_writer_.Bool(telemetry_data.calibrate_command);
  json_writer_.Key("emergency_stop");
  json_writer_.Bool(telemetry_data.emergency_stop_command);
  json_writer_.Key("launch");
  json_writer_.Bool(telemetry_data.launch_command);
  json_writer_.Key("nominal_breaking");
  json_writer_.Bool(telemetry_data.nominal_braking_command);
  json_writer_.Key("service_propulsion_go");
  json_writer_.Bool(telemetry_data.service_propulsion_go);
  json_writer_.Key("shutdown");
  json_writer_.Bool(telemetry_data.shutdown_command);

  // Module status
  json_writer_.Key("telemetry_status");
  json_writer_.String(convertModuleStatus(telemetry_data.module_status).c_str());
  json_writer_.EndObject();
}

void Writer::packNavigationData()
{
  const auto nav_data = data_.getNavigationData();

  json_writer_.Key("navigation");
  json_writer_.StartObject();
  json_writer_.Key("braking_distance");
  json_writer_.Int(nav_data.braking_distance);
  json_writer_.Key("displacement");
  json_writer_.Int(nav_data.displacement);  // Crucial
  json_writer_.Key("emergency_braking_distance");
  json_writer_.Int(nav_data.emergency_braking_distance);
  json_writer_.Key("velocity");
  json_writer_.Int(nav_data.velocity);  // Crucial
  json_writer_.Key("acceleration");
  json_writer_.Int(nav_data.acceleration);  // Crucial

  // Module status
  json_writer_.Key("navigation_status");
  json_writer_.String(convertModuleStatus(nav_data.module_status).c_str());
  json_writer_.EndObject();
}

void Writer::packSensorsData()
{
  const auto sensors_data   = data_.getSensorsData();
  const auto batteries_data = data_.getBatteriesData();
  const auto brakes_data    = data_.getEmergencyBrakesData();

  json_writer_.Key("sensors");
  json_writer_.StartObject();
  // LP Batteries
  json_writer_.Key("lp_batteries");
  json_writer_.StartArray();
  for (std::size_t i = 0; i < batteries_data.kNumLPBatteries; ++i) {
    packBattery("lp_battery", batteries_data.low_power_batteries[i]);
  }
  json_writer_.EndArray();

  // HP Batteries
  json_writer_.Key("hp_batteries");
  json_writer_.StartArray();
  for (std::size_t i = 0; i < batteries_data.kNumHPBatteries; ++i) {
    packBattery("hp_battery", batteries_data.high_power_batteries[i]);
  }
  json_writer_.EndArray();

  // Other sensor data
  json_writer_.Key("brakes_retracted");
  json_writer_.Bool(brakes_data.brakes_retracted);
  json_writer_.Key("temperature");
  json_writer_.Int(data_.getTemperature());
  // ImuData, EncoderData, StripeCounter data types not currently supported by json writer

  // Module statuses
  json_writer_.Key("brakes_status");
  json_writer_.String(convertModuleStatus(brakes_data.module_status).c_str());
  json_writer_.Key("sensors_status");
  json_writer_.Bool(convertModuleStatus(sensors_data.module_status).c_str());
  json_writer_.Key("batteries_status");
  json_writer_.Bool(convertModuleStatus(batteries_data.module_status).c_str());
  json_writer_.EndObject();
}

void Writer::packMotorData()
{
  const auto motor_data = data_.getMotorData();

  json_writer_.Key("motors");
  json_writer_.StartObject();
  // RPMS
  json_writer_.Key("motor_rpms");
  json_writer_.StartArray();
  for (std::size_t i = 0; i < motor_data.kNumMotors; ++i) {
    json_writer_.Int(motor_data.rpms[i]);
  }
  json_writer_.EndArray();

  // Module status
  json_writer_.Key("motors_status");
  json_writer_.String(convertModuleStatus(motor_data.module_status).c_str());
  json_writer_.EndObject();
}

void Writer::packStateMachineData()
{
  const auto sm_data = data_.getStateMachineData();

  json_writer_.Key("state_machine");
  json_writer_.StartObject();
  json_writer_.Key("critical_failure");
  json_writer_.Bool(sm_data.critical_failure);
  json_writer_.Key("current_state");
  json_writer_.String(convertStateMachineState(sm_data.current_state).c_str());  // Crucial
  json_writer_.EndObject();
}

void Writer::packBattery(const std::string name, const data::BatteryData &battery)
{
  json_writer_.Key(name.c_str());
  json_writer_.StartObject();
  json_writer_.Key("average_temp");
  json_writer_.Int(battery.average_temperature);
  json_writer_.Key("voltage");
  json_writer_.Int(battery.voltage);
  json_writer_.Key("current");
  json_writer_.Int(battery.current);
  json_writer_.Key("charge");
  json_writer_.Int(battery.charge);
  json_writer_.Key("low_temp");
  json_writer_.Int(battery.low_temperature);
  json_writer_.Key("high_temp");
  json_writer_.Int(battery.high_temperature);
  json_writer_.Key("low_voltage_cell");
  json_writer_.Int(battery.low_voltage_cell);
  json_writer_.Key("high_voltage_cell");
  json_writer_.Int(battery.high_voltage_cell);
  json_writer_.Key("imd_fault");
  json_writer_.Bool(battery.imd_fault);
  json_writer_.EndObject();
}

const std::string Writer::convertStateMachineState(data::State state)
{
  switch (state) {
    case data::State::kInvalid:
      return "INVALID";
    case data::State::kEmergencyBraking:
      return "EMERGENCY_BRAKING";
    case data::State::kFailureStopped:
      return "FAILURE_STOPPED";
    case data::State::kIdle:
      return "IDLE";
    case data::State::kPreCalibrating:
      return "PRE_CALIBRATING";
    case data::State::kCalibrating:
      return "CALIBRATING";
    case data::State::kFinished:
      return "FINISHED";
    case data::State::kReady:
      return "READY";
    case data::State::kAccelerating:
      return "ACCELERATING";
    case data::State::kCruising:
      return "CRUISING";
    case data::State::kNominalBraking:
      return "NOMINAL_BRAKING";
    default:
      return "";
  }
}

const std::string Writer::convertModuleStatus(data::ModuleStatus module_status)
{
  switch (module_status) {
    case data::ModuleStatus::kStart:
      return "START";
    case data::ModuleStatus::kInit:
      return "INIT";
    case data::ModuleStatus::kReady:
      return "READY";
    case data::ModuleStatus::kCriticalFailure:
      return "CRITICAL_FAILURE";
    default:
      return "";
  }
}

}  // namespace hyped::telemetry
