#pragma once

#include "sender_interface.hpp"

#include <utils/io/can.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::propulsion {
class CanSender : public ISender {
 public:
  CanSender();

  bool sendMessage(utils::io::can::Frame &message) override;

 private:
  utils::io::Can &can_;
  utils::Logger log_;
};
}  // namespace hyped::propulsion
