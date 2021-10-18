#pragma once

#include <string>

#include <data/data.hpp>
#include <rapidjson/writer.h>

namespace hyped {
namespace telemetry {

/*
  Guide on how to use this class: https://github.com/Hyp-ed/hyped-2020/wiki/Adding-new-data-points
  RapidJSON's Writer documentation: https://rapidjson.org/md_doc_sax.html#Writer

  RapidJSON works by basically going step by step and writing JSON string. The main functions are:
  * StartObject() and EndObject() start and end JSON object (basically writes curly brackets)
  * StartArray() and EndArray() start and end JSON array (basically writes square brackets)
  * Key(name) writes the key name for the value (or other object or array) that will be added in the
  next step
  * Int(value), Double(value), String(value) and other similar functions write the value for the key
  that was written one step earlier
  * GetString() returns JSON as a string

  This Writer class "extends" RapidJSON's Writer, so it's easier to use it, it takes significantly
  fewer lines of code to include all the required information about the each data point and
  everything is formatted correctly, so that GUI could parse it.
*/
class Writer {
 public:
  explicit Writer(data::Data &data);

  // Separate functions that allow to better manage data points based on their purpose
  void packTime();
  void packCrucialData();
  void packStatusData();
  void packAdditionalData();

  // Before starting adding data points, this function must be called to start the main JSON object
  void start() { rjwriter_.StartObject(); }

  // Before calling getString(), this function must be called to close the main JSON object
  // After calling this function, no additional data points can be added
  void end() { rjwriter_.EndObject(); }

  // Returns the main JSON object as a string, that is ready to be sent to GUI
  std::string getString() { return sb_.GetString(); }

  // Converts Enum to String values with required formatting for GUI
  static const char *convertStateMachineState(data::State state);
  static const char *convertModuleStatus(data::ModuleStatus module_status);

 private:
  // Calls RapidJSON functions to add a value of specific type to JSON
  void add(const char *name, int min, int max, const char *unit, int value);
  void add(const char *name, float min, float max, const char *unit, float value);
  void add(const char *name, bool value);
  void add(const char *name, const char *value);
  void add(const char *name, data::State value);
  void add(const char *name, data::ModuleStatus value);

  // Starts and ends lists, which allow to structure the data
  void startList(const char *name);
  void endList();

  rapidjson::StringBuffer sb_;
  rapidjson::Writer<rapidjson::StringBuffer> rjwriter_;
  data::Data &data_;
};

}  // namespace telemetry
}  // namespace hyped
