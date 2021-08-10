#pragma once

#include "client.hpp"

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped {

using utils::Logger;
using utils::concurrent::Thread;

namespace telemetry {

class Main : public Thread {
 public:
  Main(uint8_t id, Logger &log);
  void run() override;

 private:
  friend class SendLoop;
  friend class RecvLoop;
  data::Data &data_;
  Client client_;
};

}  // namespace telemetry
}  // namespace hyped
