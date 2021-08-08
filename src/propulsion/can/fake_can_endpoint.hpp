#pragma once

#include <utils/concurrent/thread.hpp>
#include <utils/io/can.hpp>

namespace hyped {

using utils::concurrent::Thread;
using utils::io::CanProccesor;
using utils::io::can::Frame;

namespace motor_control {

class FakeCanEndpoint : public Thread {
 public:
  explicit FakeCanEndpoint(CanProccesor *sender);
  void run() override;

 private:
  CanProccesor *sender_;
};
}  // namespace motor_control
}  // namespace hyped
