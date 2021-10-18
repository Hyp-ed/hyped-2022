#pragma once

#include "client.hpp"

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped {

namespace telemetry {

class Main : public utils::concurrent::Thread {
 public:
  Main(uint8_t id, utils::Logger &log);
  void run() override;

 private:
  friend class SendLoop;
  friend class RecvLoop;
  data::Data &data_;
  Client client_;
};

}  // namespace telemetry
}  // namespace hyped
