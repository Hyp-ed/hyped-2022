#pragma once

#include "main.hpp"

#include <string>

#include <rapidjson/writer.h>

#include "utils/system.hpp"
#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::telemetry {

class Sender : public utils::concurrent::Thread {
 public:
  explicit Sender(data::Data &data, Client &client);
  void run() override;

 private:
  utils::System &sys_;
  data::Data &data_;
  Client &client_;
  std::string convertStateMachineState(data::State state);
  std::string convertModuleStatus(data::ModuleStatus module_status);
};

}  // namespace hyped::telemetry
