#pragma once

#include <utils/io/can.hpp>

namespace hyped {

namespace propulsion {

class ICanReceiver {
 public:
  /**
   * @brief { Registers the controller to process incoming CAN messages}
   * */
  virtual void registerController() = 0;

  virtual void processNewData(utils::io::can::Frame &message) = 0;

  virtual bool hasId(const uint32_t id, bool extended) = 0;
};

}  // namespace propulsion
}  // namespace hyped
