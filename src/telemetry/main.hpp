#pragma once

#include "client.hpp"

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::telemetry {

class Main : public utils::concurrent::Thread {
 public:
  Main();
  void run() override;

 private:
  friend class SendLoop;
  friend class RecvLoop;
  data::Data &data_;
  std::unique_ptr<Client> client_;
};

}  // namespace hyped::telemetry
