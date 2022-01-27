#pragma once

#include "main.hpp"

#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped::telemetry {

class Receiver : public utils::concurrent::Thread {
 public:
  explicit Receiver(data::Data &data, Client &client);
  void run() override;

 private:
  data::Data &data_;
  Client &client_;
};

}  // namespace hyped::telemetry
