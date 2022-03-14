#pragma once

#include "fake_can_endpoint.hpp"
#include "sender_interface.hpp"

#include <atomic>
#include <iostream>

#include <utils/io/can.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::propulsion {

class FakeCanSender : public utils::io::CanProccesor, public ISender {
 public:
  FakeCanSender(uint8_t id);

  bool sendMessage(utils::io::can::Frame &message) override;

  void registerController() override;

  void processNewData(utils::io::can::Frame &message) override;

  bool hasId(uint32_t id, bool extended) override;

  bool getIsSending();

 private:
  utils::Logger log_;
  // Can& can_;
  std::atomic<bool> is_sending_;
  FakeCanEndpoint can_endpoint_;
};

}  // namespace hyped::propulsion
