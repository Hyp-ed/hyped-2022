#pragma once

#include "sender_interface.hpp"

#include <utils/io/can.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::propulsion {
class CanSender {
 public:
  CanSender(utils::io::Can &can);

  bool sendMessage(utils::io::can::Frame &message);

 private:
  utils::Logger log_;
  utils::io::Can &can_;
};
}  // namespace hyped::propulsion
