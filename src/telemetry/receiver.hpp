#pragma once

#include "main.hpp"

#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped::telemetry {

class Receiver : public utils::concurrent::Thread {
 public:
  explicit Receiver(utils::Logger &log, data::Data &data, Client &client);
  void run() override;

 private:
  Client &client_;
  data::Data &data_;
};

}  // namespace hyped::telemetry
