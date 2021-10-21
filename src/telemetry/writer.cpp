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
  rapid_json_writer_.Key("time");
  rapid_json_writer_.Uint64(std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count());
}

// Ddditional data points that are displayed in the GUI data section
// FEEL FREE TO EDIT. More info: https://github.com/Hyp-ed/hyped-2020/wiki/Adding-new-data-points
void Writer::packAdditionalData()
{
  rapid_json_writer_.Key("additional_data");
  rapid_json_writer_.StartArray();

  // Edit below

  // Edit above

  rapid_json_writer_.EndArray();
}

// Crucial data points that are displayed in various fixed GUI points
// NOT EDITABLE
void Writer::packCrucialData()
{
  rapid_json_writer_.Key("crucial_data");
  rapid_json_writer_.StartArray();

  data::Navigation nav_data = data_.getNavigationData();
  data::StateMachine sm_data = data_.getStateMachineData();
  add("distance", 0.0, 1250.0, "m", nav_data.displacement);
  add("velocity", 0.0, 250.0, "m/s", nav_data.velocity);
  add("acceleration", -50.0, 50.0, "m/s^2", nav_data.acceleration);
  add("status", sm_data.current_state);

  rapid_json_writer_.EndArray();
}

// Status data points that are displayed in the GUI status tab
// NOT EDITABLE
void Writer::packStatusData()
{
  rapid_json_writer_.Key("status_data");
  rapid_json_writer_.StartArray();

  // TODO(everyone): add all required data points

  rapid_json_writer_.EndArray();
}

Writer::Writer(data::Data &data) : rapid_json_writer_(sb_), data_{data}
{
}

void Writer::startList(const char *name)
{
  rapid_json_writer_.StartObject();
  rapid_json_writer_.Key("name");
  rapid_json_writer_.String(name);
  rapid_json_writer_.Key("value");
  rapid_json_writer_.StartArray();
}

void Writer::endList()
{
  rapid_json_writer_.EndArray();
  rapid_json_writer_.EndObject();
}

void Writer::add(const char *name, int min, int max, const char *unit, int value)
{
  rapid_json_writer_.StartObject();
  rapid_json_writer_.Key("name");
  rapid_json_writer_.String(name);
  rapid_json_writer_.Key("min");
  rapid_json_writer_.Int(min);
  rapid_json_writer_.Key("max");
  rapid_json_writer_.Int(max);
  rapid_json_writer_.Key("unit");
  rapid_json_writer_.String(unit);
  rapid_json_writer_.Key("value");
  rapid_json_writer_.Int(value);
  rapid_json_writer_.EndObject();
}

void Writer::add(const char *name, float min, float max, const char *unit, float value)
{
  rapid_json_writer_.StartObject();
  rapid_json_writer_.Key("name");
  rapid_json_writer_.String(name);
  rapid_json_writer_.Key("min");
  rapid_json_writer_.Double(min);
  rapid_json_writer_.Key("max");
  rapid_json_writer_.Double(max);
  rapid_json_writer_.Key("unit");
  rapid_json_writer_.String(unit);
  rapid_json_writer_.Key("value");
  rapid_json_writer_.Double(value);
  rapid_json_writer_.EndObject();
}

void Writer::add(const char *name, bool value)
{
  rapid_json_writer_.StartObject();
  rapid_json_writer_.Key("name");
  rapid_json_writer_.String(name);
  rapid_json_writer_.Key("value");
  rapid_json_writer_.Bool(value);
  rapid_json_writer_.EndObject();
}

void Writer::add(const char *name, data::State value)
{
  rapid_json_writer_.StartObject();
  rapid_json_writer_.Key("name");
  rapid_json_writer_.String(name);
  rapid_json_writer_.Key("value");
  rapid_json_writer_.String(convertStateMachineState(value));
  rapid_json_writer_.EndObject();
}

void Writer::add(const char *name, data::ModuleStatus value)
{
  rapid_json_writer_.StartObject();
  rapid_json_writer_.Key("name");
  rapid_json_writer_.String(name);
  rapid_json_writer_.Key("value");
  rapid_json_writer_.String(convertModuleStatus(value));
  rapid_json_writer_.EndObject();
}

const char *Writer::convertStateMachineState(data::State state)
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

const char *Writer::convertModuleStatus(data::ModuleStatus module_status)
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
