#pragma once

#include "main.hpp"

#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped {

namespace telemetry {

class ReceiveLoop : public utils::concurrent::Thread {
 public:
  explicit ReceiveLoop(utils::Logger &log, data::Data &data, Client &client);
  void run() override;

 private:
  Client &client_;
  data::Data &data_;
};

}  // namespace telemetry
}  // namespace hyped
