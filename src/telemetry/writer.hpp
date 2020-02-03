/*
 * Author: Robertas Norkus
 * Organisation: HYPED
 * Date: January 2020
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

#ifndef TELEMETRY_WRITER_HPP_
#define TELEMETRY_WRITER_HPP_

#include <rapidjson/writer.h>
#include <string>
#include "data/data.hpp"

namespace hyped {
namespace telemetry {

class Writer {
 public:
  explicit Writer(data::Data& data);
  void packCrucialData();
  void packStatusData();
  void packAdditionalData();

  void start()
  {
    rjwriter_.StartObject();
  }
  void end()
  {
    rjwriter_.EndObject();
  }
  std::string getString()
  {
    return sb_.GetString();
  }

 private:
  void add(const char* name, int min, int max, const char* unit, int value);
  void add(const char* name, float min, float max, const char* unit, float value);
  void add(const char* name, bool value);
  void add(const char* name, const char* value);
  void add(const char* name, data::State value);
  void add(const char* name, data::ModuleStatus value);
  void startList(const char* name);
  void endList();

  rapidjson::StringBuffer sb_;
  rapidjson::Writer<rapidjson::StringBuffer> rjwriter_;
  const char* convertStateMachineState(data::State state);
  const char* convertModuleStatus(data::ModuleStatus module_status);
  data::Data& data_;
};

}  // namespace telemetry
}  // namespace hyped

#endif  // TELEMETRY_WRITER_HPP_
