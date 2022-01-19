#pragma once

#include "main.hpp"

#include <string>

#include <data/data.hpp>
#include <rapidjson/writer.h>
#include <utils/concurrent/thread.hpp>

namespace hyped::telemetry {

class Sender : public utils::concurrent::Thread {
 public:
  explicit Sender(utils::Logger &log, data::Data &data, Client &client);
  void run() override;

 private:
  data::Data &data_;
  Client &client_;
  std::string convertStateMachineState(data::State state);
  std::string convertModuleStatus(data::ModuleStatus module_status);
};

}  // namespace hyped::telemetry
