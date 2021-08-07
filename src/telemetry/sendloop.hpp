#pragma once

#include <rapidjson/writer.h>

#include <data/data.hpp>
#include <string>
#include <utils/concurrent/thread.hpp>

#include "main.hpp"

using rapidjson::StringBuffer;
using rapidjson::Writer;

namespace hyped {

using utils::Logger;
using utils::concurrent::Thread;

namespace telemetry {

class SendLoop : public Thread {
 public:
  explicit SendLoop(Logger &log, data::Data &data, Main *main_pointer);
  void run() override;

 private:
  std::string convertStateMachineState(data::State state);
  std::string convertModuleStatus(data::ModuleStatus module_status);
  Main &main_ref_;
  data::Data &data_;
};

}  // namespace telemetry
}  // namespace hyped
