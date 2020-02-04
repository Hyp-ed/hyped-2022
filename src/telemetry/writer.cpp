/*
 * Author: Robertas Norkus
 * Organisation: HYPED
 * Date: January 2020
 * Description:
 *
 *    Copyright 2020 HYPED
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

#include "writer.hpp"
#include "data/data.hpp"

namespace hyped {
namespace telemetry {

// additional data points that are displayed in the GUI data section
// FEEL FREE TO EDIT. More info: https://github.com/Hyp-ed/hyped-2020/wiki/Adding-new-data-points
void Writer::packAdditionalData()
{
  rjwriter_.Key("additional_data");
  rjwriter_.StartArray();

  // edit below

  // edit above

  rjwriter_.EndArray();
}

// crucial data points that are displayed in various fixed GUI points
// NOT EDITABLE
void Writer::packCrucialData()
{
  rjwriter_.Key("crucial_data");
  rjwriter_.StartArray();

  data::Navigation nav_data = data_.getNavigationData();
  data::StateMachine sm_data = data_.getStateMachineData();
  add("distance", 0.0, 1250.0, "m", nav_data.distance);
  add("velocity", 0.0, 250.0, "m/s", nav_data.velocity);
  add("acceleration", -50.0, 50.0, "m/s^2", nav_data.acceleration);
  add("status", sm_data.current_state);

  rjwriter_.EndArray();
}

// status data points that are displayed in the GUI status tab
// NOT EDITABLE
void Writer::packStatusData()
{
  rjwriter_.Key("status_data");
  rjwriter_.StartArray();

  // TODO(everyone): add all required data points

  rjwriter_.EndArray();
}

Writer::Writer(data::Data& data)
  : rjwriter_(sb_),
    data_ {data}
{}

void Writer::startList(const char* name)
{
  rjwriter_.StartObject();
  rjwriter_.Key("name");
  rjwriter_.String(name);
  rjwriter_.Key("value");
  rjwriter_.StartArray();
}

void Writer::endList()
{
  rjwriter_.EndArray();
  rjwriter_.EndObject();
}

void Writer::add(const char* name, int min, int max, const char* unit, int value)
{
  rjwriter_.StartObject();
  rjwriter_.Key("name");
  rjwriter_.String(name);
  rjwriter_.Key("min");
  rjwriter_.Int(min);
  rjwriter_.Key("max");
  rjwriter_.Int(max);
  rjwriter_.Key("unit");
  rjwriter_.String(unit);
  rjwriter_.Key("value");
  rjwriter_.Int(value);
  rjwriter_.EndObject();
}

void Writer::add(const char* name, float min, float max, const char* unit, float value)
{
  rjwriter_.StartObject();
  rjwriter_.Key("name");
  rjwriter_.String(name);
  rjwriter_.Key("min");
  rjwriter_.Double(min);
  rjwriter_.Key("max");
  rjwriter_.Double(max);
  rjwriter_.Key("unit");
  rjwriter_.String(unit);
  rjwriter_.Key("value");
  rjwriter_.Double(value);
  rjwriter_.EndObject();
}

void Writer::add(const char* name, bool value)
{
  rjwriter_.StartObject();
  rjwriter_.Key("name");
  rjwriter_.String(name);
  rjwriter_.Key("value");
  rjwriter_.Bool(value);
  rjwriter_.EndObject();
}

void Writer::add(const char* name, data::State value)
{
  rjwriter_.StartObject();
  rjwriter_.Key("name");
  rjwriter_.String(name);
  rjwriter_.Key("value");
  rjwriter_.String(convertStateMachineState(value));
  rjwriter_.EndObject();
}

void Writer::add(const char* name, data::ModuleStatus value)
{
  rjwriter_.StartObject();
  rjwriter_.Key("name");
  rjwriter_.String(name);
  rjwriter_.Key("value");
  rjwriter_.String(convertModuleStatus(value));
  rjwriter_.EndObject();
}

const char* Writer::convertStateMachineState(data::State state)
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
    case data::State::kRunComplete:
      return "RUN_COMPLETE";
    case data::State::kFinished:
      return "FINISHED";
    case data::State::kReady:
      return "READY";
    case data::State::kAccelerating:
      return "ACCELERATING";
    case data::State::kNominalBraking:
      return "NOMINAL_BRAKING";
    case data::State::kExiting:
      return "EXITING";
    default:
      return "";
    }
}

const char* Writer::convertModuleStatus(data::ModuleStatus module_status)
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
