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

  // Edit below

  // Edit above

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

  // TODO(everyone): add all required data points

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
