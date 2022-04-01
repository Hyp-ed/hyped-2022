#pragma once

#include <utils/concurrent/thread.hpp>
#include <utils/io/can.hpp>

namespace hyped::propulsion {

class FakeCanEndpoint : public utils::concurrent::Thread {
 public:
  explicit FakeCanEndpoint(utils::io::CanProcessor &sender);
  void run() override;

 private:
  utils::io::CanProcessor &sender_;
};

}  // namespace hyped::propulsion
