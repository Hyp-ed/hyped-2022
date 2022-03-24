#pragma once

#include "sender_interface.hpp"

#include <utils/io/can.hpp>
#include <utils/logger.hpp>

namespace hyped::propulsion {
class CanSender : public ICanSender {
 public:
  CanSender();

  bool sendMessage(const utils::io::can::Frame &message) override;

 private:
  utils::io::Can &can_;
  utils::Logger log_;
};
}  // namespace hyped::propulsion
