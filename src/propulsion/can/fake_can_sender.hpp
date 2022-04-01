#pragma once

#include "fake_can_endpoint.hpp"
#include "sender_interface.hpp"

#include <atomic>
#include <iostream>

#include <utils/io/can.hpp>
#include <utils/logger.hpp>

namespace hyped::propulsion {

class FakeCanSender : public utils::io::CanProcessor, public ISender {
 public:
  FakeCanSender(utils::Logger &log_, uint8_t id);

  bool sendMessage(utils::io::can::Frame &message) override;

  void registerController() override;

  void processNewData(utils::io::can::Frame &message) override;

  bool hasId(uint32_t id, bool extended) override;

  bool getIsSending() override;

 private:
  utils::Logger log_;
  // Can& can_;
  std::atomic<bool> is_sending_;
  FakeCanEndpoint can_endpoint_;
};

}  // namespace hyped::propulsion
