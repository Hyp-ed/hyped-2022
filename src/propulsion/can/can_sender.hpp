#pragma once

#include "sender_interface.hpp"

#include <utils/io/can.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::propulsion {
class CanSender {
 public:
  CanSender();

  bool sendMessage(utils::io::can::Frame &message);

 protected:
  utils::io::Can &can_;

 private:
  utils::Logger log_;
};
}  // namespace hyped::propulsion
