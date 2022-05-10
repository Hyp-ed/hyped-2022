#pragma once

#include "fake_can_endpoint.hpp"
#include "receiver_interface.hpp"

#include <atomic>
#include <iostream>

#include <utils/io/can.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::propulsion {

class FakeCanReceiver : public utils::io::ICanProccessor, public ICanReceiver {
 public:
  FakeCanReceiver();

  void registerController() override;

  void processNewData(utils::io::can::Frame &message) override;

  bool hasId(const uint32_t id, bool extended) override;

  bool getIsSending();

 private:
  utils::Logger log_;
  // Can& can_;
  std::atomic<bool> is_sending_;
  FakeCanEndpoint can_endpoint_;
};

}  // namespace hyped::propulsion
