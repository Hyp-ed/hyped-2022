#pragma once

#include <utils/concurrent/thread.hpp>
#include <utils/io/can.hpp>

namespace hyped::propulsion {

class FakeCanEndpoint : public utils::concurrent::Thread {
 public:
  explicit FakeCanEndpoint(utils::io::ICanProcessor &sender);
  void run() override;

 private:
  utils::io::ICanProcessor &sender_;
};

}  // namespace hyped::propulsion
