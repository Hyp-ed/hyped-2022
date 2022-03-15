#pragma once

#include <utils/io/can.hpp>

namespace hyped {

namespace propulsion {

class ICanSender {
 public:
  /**
   * @brief { Sends CAN messages }
   */
  virtual bool sendMessage(utils::io::can::Frame &message) = 0;
};

}  // namespace propulsion
}  // namespace hyped
