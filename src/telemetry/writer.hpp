/*
 * Author: Robertas Norkus
 * Organisation: HYPED
 * Date: January 2020
 * Description:
 * This is a wrapper class, that is built on top of RapidJSON's Writer and allows to more easily (and with required formatting)
 * write data to the JSON, which is sent to GUI.
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

#ifndef TELEMETRY_WRITER_HPP_
#define TELEMETRY_WRITER_HPP_

#include <rapidjson/writer.h>
#include <string>
#include "data/data.hpp"

namespace hyped {
namespace telemetry {

/*
  Guide on how to use this class: https://github.com/Hyp-ed/hyped-2020/wiki/Adding-new-data-points
  RapidJSON's Writer documentation: https://rapidjson.org/md_doc_sax.html#Writer

  RapidJSON works by basically going step by step and writing JSON string. The main functions are:
  * StartObject() and EndObject() start and end JSON object (basically writes curly brackets)
  * StartArray() and EndArray() start and end JSON array (basically writes square brackets)
  * Key(name) writes the key name for the value (or other object or array) that will be added in the next step
  * Int(value), Double(value), String(value) and other similar functions write the value for the key that was written one step earlier
  * GetString() returns JSON as a string

  This Writer class "extends" RapidJSON's Writer, so it's easier to use it, it takes significantly fewer lines of code
  to include all the required information about the each data point and everything is formatted correctly, so that GUI could parse it.
*/
class Writer {
 public:
  explicit Writer(data::Data& data);

  // separate functions that allow to better manage data points based on their purpose
  void packCrucialData();
  void packStatusData();
  void packAdditionalData();

  // before starting adding data points, this function must be called to start the main JSON object
  void start()
  {
    rjwriter_.StartObject();
  }

  // before calling getString(), this function must be called to close the main JSON object
  // after calling this function, no additional data points can be added
  void end()
  {
    rjwriter_.EndObject();
  }

  // returns the main JSON object as a string, that is ready to be sent to GUI
  std::string getString()
  {
    return sb_.GetString();
  }

 private:
  // calls RapidJSON functions to add a value of specific type to JSON
  void add(const char* name, int min, int max, const char* unit, int value);
  void add(const char* name, float min, float max, const char* unit, float value);
  void add(const char* name, bool value);
  void add(const char* name, const char* value);
  void add(const char* name, data::State value);
  void add(const char* name, data::ModuleStatus value);

  // starts and ends lists, which allow to structure the data
  void startList(const char* name);
  void endList();

  // converts Enum to String values with required formatting for GUI
  const char* convertStateMachineState(data::State state);
  const char* convertModuleStatus(data::ModuleStatus module_status);

  rapidjson::StringBuffer sb_;
  rapidjson::Writer<rapidjson::StringBuffer> rjwriter_;
  data::Data& data_;
};

}  // namespace telemetry
}  // namespace hyped

#endif  // TELEMETRY_WRITER_HPP_
