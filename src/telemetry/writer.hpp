#pragma once

#include <string>

#include <rapidjson/writer.h>

#include <data/data.hpp>

namespace hyped::telemetry {

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
  explicit Writer();

  // functions to pack timestamp and number of packages
  void packTime();
  void packId(const uint16_t id);

  // specific functions that allow packing of structs in central data structure
  void packTelemetryData();
  void packSensorsData();
  void packMotorData();
  void packStateMachineData();
  void packNavigationData();

  // before starting adding data points, this function must be called to start the main JSON object
  void start() { json_writer_.StartObject(); }

  // before calling getString(), this function must be called to close the main JSON object
  // after calling this function, no additional data points can be added
  void end() { json_writer_.EndObject(); }

  // checks whether json is complete. json is complete if it has complete root object or array.
  bool isValidJson() { return json_writer_.IsComplete(); }

  // returns the main JSON object as a string, that is ready to be sent to GUI
  std::string getString() { return string_buffer_.GetString(); }

  // converts Enum to String values with required formatting for GUI
  static const std::string convertStateMachineState(data::State state);
  static const std::string convertModuleStatus(data::ModuleStatus module_status);

 private:
  // functions to pack internal CDS structs/types
  void packBattery(const data::BatteryData &battery);

  rapidjson::StringBuffer string_buffer_;
  rapidjson::Writer<rapidjson::StringBuffer> json_writer_;
  data::Data &data_;
};

}  // namespace hyped::telemetry
