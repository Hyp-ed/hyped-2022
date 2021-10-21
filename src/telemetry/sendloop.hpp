#pragma once

#include "main.hpp"

#include <string>

#include <data/data.hpp>
#include <rapidjson/writer.h>
#include <utils/concurrent/thread.hpp>

namespace hyped {

namespace telemetry {

class SendLoop : public utils::concurrent::Thread {
 public:
  explicit SendLoop(utils::Logger &log, data::Data &data, Main &main_ref);
  void run() override;

 private:
  std::string convertStateMachineState(data::State state);
  std::string convertModuleStatus(data::ModuleStatus module_status);
  Main &main_ref_;
  data::Data &data_;
};

}  // namespace telemetry
}  // namespace hyped
