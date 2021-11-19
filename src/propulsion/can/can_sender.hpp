#pragma once

#include <atomic>
#include <iostream>

#include "sender_interface.hpp"
#include <propulsion/controller_interface.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/io/can.hpp>
#include <utils/logger.hpp>
#include <utils/timer.hpp>

#define TIMEOUT 70000

namespace hyped {
namespace motor_control {
using utils::Logger;
using utils::Timer;
using utils::concurrent::Thread;
using utils::io::Can;
using utils::io::CanProccesor;

class CanSender : public CanProccesor, public SenderInterface {
 public:
  /**
   * @brief { Initialise the CanSender with the logger and the id }
   */
  CanSender(Logger &log_, uint8_t node_id);

  /**
   * @brief { Initialise the CanSender with the logger, the id and the controller as an attribute,
   * to access it's attributes }
   */
  CanSender(ControllerInterface *controller, uint8_t node_id, Logger &log_);

  /**
   * @brief { Sends CAN messages }
   */
  bool sendMessage(utils::io::can::Frame &message) override;

  /**
   * @brief { Registers the controller to process incoming CAN messages }
   */
  void registerController() override;

  /**
   * @brief { This function processes incoming CAN messages }
   */
  void processNewData(utils::io::can::Frame &message) override;

  /**
   * @brief { If this function returns true, the CAN message is ment for this CAN node }
   */
  bool hasId(uint32_t id, bool) override;

  /**
   * @brief { Return if the can_sender is sending a CAN message right now }
   */
  bool getIsSending() override;

 private:
  Logger &log_;
  uint8_t node_id_;
  Can &can_;
  std::atomic<bool> isSending;
  ControllerInterface *controller_;
  Timer timer;
  uint64_t messageTimestamp;

  const uint32_t kEmgyTransmit = 0x80;
  const uint32_t kSdoTransmit  = 0x580;
  const uint32_t kNmtTransmit  = 0x700;
};
}  // namespace motor_control
}  // namespace hyped
