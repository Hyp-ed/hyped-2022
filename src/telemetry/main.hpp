#pragma once

#include "client.hpp"

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::telemetry {

class Main : public utils::concurrent::Thread {
 public:
  Main(const uint8_t id, utils::Logger &log);
  void run() override;

 private:
  data::Data &data_;
  Client client_;
};

}  // namespace hyped::telemetry
