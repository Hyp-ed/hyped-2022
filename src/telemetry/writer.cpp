#include "writer.hpp"

#include <chrono>
#include <cstdint>
#include <string>

#include <data/data.hpp>

namespace hyped {
namespace telemetry {

// The current time in milliseconds that will be used later
void Writer::packTime()
{
  json_writer_.Key("time");
  json_writer_.Uint64(std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count());
}

// Additional data points that are displayed in the GUI data section
// FEEL FREE TO EDIT. More info: https://github.com/Hyp-ed/hyped-2020/wiki/Adding-new-data-points
void Writer::packAdditionalData()
{
  json_writer_.Key("additional_data");
  json_writer_.StartArray();

  // Telemetry
  data::Telemetry tel_data = data_.getTelemetryData();
  startList("Telemetry");
  add("calibrate", tel_data.calibrate_command);
  add("emergency_stop", tel_data.emergency_stop_command);
  add("launch", tel_data.launch_command);
  add("nominal_braking", tel_data.nominal_braking_command);
  add("service_propulsion_go", tel_data.service_propulsion_go);
  add("shutdown", tel_data.shutdown_command);
  endList();

  // Navigation
  data::Navigation nav_data = data_.getNavigationData();
  startList("Navigation");
  add("braking_distance", 0.0, 1250.0, "m", nav_data.braking_distance);
  add("displacement", 0.0, 1250.0, "m", nav_data.displacement);
  add("emergency_braking_distance", 0.0, 1250.0, "m", nav_data.emergency_braking_distance);
  endList();

  // Sensors (emergency brakes and batteries included)
  data::Sensors sensors_data        = data_.getSensorsData();
  data::Batteries batteries_data    = data_.getBatteriesData();
  data::EmergencyBrakes brakes_data = data_.getEmergencyBrakesData();
  startList("Sensors");
  add("brakes_retracted", brakes_data.brakes_retracted);
  add("temperature", data_.getTemperature());

  startList("Low Power Batteries");
  for (std::size_t i = 0; i < batteries_data.kNumLPBatteries; ++i) {
    std::string battery_num = std::to_string(i);
    startList("Low Power Battery " + battery_num);
    add("lp_battery_" + battery_num + "_ave_temp",
        batteries_data.low_power_batteries[i].average_temperature);
    add("lp_battery_" + battery_num + "_volt", batteries_data.low_power_batteries[i].voltage);
    add("lp_battery_" + battery_num + "_current", batteries_data.low_power_batteries[i].current);
    add("lp_battery_" + battery_num + "_charge", batteries_data.low_power_batteries[i].charge);
    add("lp_battery_" + battery_num + "_low_temp",
        batteries_data.low_power_batteries[i].low_temperature);
    add("lp_battery_" + battery_num + "_high_temp",
        batteries_data.low_power_batteries[i].high_temperature);
    add("lp_battery_" + battery_num + "_low_volt_cell",
        batteries_data.low_power_batteries[i].low_voltage_cell);
    add("lp_battery_" + battery_num + "_high_volt_cell",
        batteries_data.low_power_batteries[i].high_voltage_cell);
    add("lp_battery_" + battery_num + "_imd_fault",
        batteries_data.low_power_batteries[i].imd_fault);
    endList();
  }
  endList();

  startList("High Power Batteries");
  for (std::size_t i = 0; i < batteries_data.kNumHPBatteries; ++i) {
    std::string battery_num = std::to_string(i);
    startList("High Power Battery " + battery_num);
    add("lp_battery_" + battery_num + "_ave_temp",
        batteries_data.high_power_batteries[i].average_temperature);
    add("lp_battery_" + battery_num + "_volt", batteries_data.high_power_batteries[i].voltage);
    add("lp_battery_" + battery_num + "_current", batteries_data.high_power_batteries[i].current);
    add("lp_battery_" + battery_num + "_charge", batteries_data.high_power_batteries[i].charge);
    add("lp_battery_" + battery_num + "_low_temp",
        batteries_data.high_power_batteries[i].low_temperature);
    add("lp_battery_" + battery_num + "_high_temp",
        batteries_data.high_power_batteries[i].high_temperature);
    add("lp_battery_" + battery_num + "_low_volt_cell",
        batteries_data.high_power_batteries[i].low_voltage_cell);
    add("lp_battery_" + battery_num + "_high_volt_cell",
        batteries_data.high_power_batteries[i].high_voltage_cell);
    add("lp_battery_" + battery_num + "_imd_fault",
        batteries_data.high_power_batteries[i].imd_fault);
    endList();
  }
  endList();

  // ImuData, EncoderData, StripeCounter data types not currently supported by json writer

  endList();

  // Motors
  data::Motors motor_data = data_.getMotorData();
  startList("Motors");
  add("motor_rpm_1", motor_data.rpms[0]);
  add("motor_rpm_2", motor_data.rpms[1]);
  add("motor_rpm_3", motor_data.rpms[2]);
  add("motor_rpm_4", motor_data.rpms[3]);
  endList();

  // State machine TODO
  data::StateMachine sm_data = data_.getStateMachineData();
  startList("State Machine");
  add("critical_failure", sm_data.critical_failure);
  add("current_state", sm_data.current_state);
  endList();

  json_writer_.EndArray();
}

// Crucial data points that are displayed in various fixed GUI points
// NOT EDITABLE
void Writer::packCrucialData()
{
  json_writer_.Key("crucial_data");
  json_writer_.StartArray();

  data::Navigation nav_data  = data_.getNavigationData();
  data::StateMachine sm_data = data_.getStateMachineData();
  add("distance", 0.0, 1250.0, "m", nav_data.displacement);
  add("velocity", 0.0, 250.0, "m/s", nav_data.velocity);
  add("acceleration", -50.0, 50.0, "m/s^2", nav_data.acceleration);
  add("status", sm_data.current_state);

  json_writer_.EndArray();
}

// Status data points that are displayed in the GUI status tab
// NOT EDITABLE
void Writer::packStatusData()
{
  json_writer_.Key("status_data");
  json_writer_.StartArray();

  // Module statuses
  data::Navigation nav_data = data_.getNavigationData();
  add("nav_status", nav_data.module_status);
  data::Motors motors_data = data_.getMotorData();
  add("motors_status", motors_data.module_status);
  data::Sensors sensors_data = data_.getSensorsData();
  add("sensors_status", sensors_data.module_status);
  data::Telemetry telemetry_data = data_.getTelemetryData();
  add("telemetry_status", telemetry_data.module_status);
  data::EmergencyBrakes brakes_data = data_.getEmergencyBrakesData();
  add("emergency_brakes_status", brakes_data.module_status);
  data::Batteries batteries_data = data_.getBatteriesData();
  add("batteries_status", batteries_data.module_status);

  json_writer_.EndArray();
}

Writer::Writer(data::Data &data) : json_writer_(string_buffer_), data_{data}
{
}

void Writer::startList(const std::string name)
{
  json_writer_.StartObject();
  json_writer_.Key("name");
  json_writer_.String(name.c_str());
  json_writer_.Key("value");
  json_writer_.StartArray();
}

void Writer::endList()
{
  json_writer_.EndArray();
  json_writer_.EndObject();
}

void Writer::add(const std::string name, int min, int max, const std::string unit, int value)
{
  json_writer_.StartObject();
  json_writer_.Key("name");
  json_writer_.String(name.c_str());
  json_writer_.Key("min");
  json_writer_.Int(min);
  json_writer_.Key("max");
  json_writer_.Int(max);
  json_writer_.Key("unit");
  json_writer_.String(unit.c_str());
  json_writer_.Key("value");
  json_writer_.Int(value);
  json_writer_.EndObject();
}

void Writer::add(const std::string name, float min, float max, const std::string unit, float value)
{
  json_writer_.StartObject();
  json_writer_.Key("name");
  json_writer_.String(name.c_str());
  json_writer_.Key("min");
  json_writer_.Double(min);
  json_writer_.Key("max");
  json_writer_.Double(max);
  json_writer_.Key("unit");
  json_writer_.String(unit.c_str());
  json_writer_.Key("value");
  json_writer_.Double(value);
  json_writer_.EndObject();
}

void Writer::add(const std::string name, bool value)
{
  json_writer_.StartObject();
  json_writer_.Key("name");
  json_writer_.String(name.c_str());
  json_writer_.Key("value");
  json_writer_.Bool(value);
  json_writer_.EndObject();
}

void Writer::add(const std::string name, data::State value)
{
  json_writer_.StartObject();
  json_writer_.Key("name");
  json_writer_.String(name.c_str());
  json_writer_.Key("value");
  json_writer_.String(convertStateMachineState(value).c_str());
  json_writer_.EndObject();
}

void Writer::add(const std::string name, data::ModuleStatus value)
{
  json_writer_.StartObject();
  json_writer_.Key("name");
  json_writer_.String(name.c_str());
  json_writer_.Key("value");
  json_writer_.String(convertModuleStatus(value).c_str());
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

}  // namespace telemetry
}  // namespace hyped
