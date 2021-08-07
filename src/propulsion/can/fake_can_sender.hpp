#pragma once

#include <atomic>
#include <iostream>
#include <utils/io/can.hpp>
#include <utils/logger.hpp>

#include "fake_can_endpoint.hpp"
#include "sender_interface.hpp"

namespace hyped {
namespace motor_control {
using utils::Logger;
using utils::io::Can;
using utils::io::CanProccesor;

class FakeCanSender : public CanProccesor, public SenderInterface {
 public:
  FakeCanSender(Logger &log_, uint8_t id);

  bool sendMessage(utils::io::can::Frame &message) override;

  void registerController() override;

  void processNewData(utils::io::can::Frame &message) override;

  bool hasId(uint32_t id, bool extended) override;

  bool getIsSending() override;

 private:
  Logger log_;
  // Can& can_;
  std::atomic<bool> isSending;
  FakeCanEndpoint *endpoint;
};
}  // namespace motor_control
}  // namespace hyped
