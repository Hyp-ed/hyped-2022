#pragma once

#include "can_sender.hpp"
#include "sender_interface.hpp"

#include <atomic>
#include <iostream>

#include <propulsion/controller_interface.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/io/can.hpp>
#include <utils/logger.hpp>

namespace hyped::propulsion {

class CanTransceiver : public utils::io::CanProccesor, public CanSender, public ISender {
 public:
  /**
   * @brief Initialise the CanTransceiver with the id and the controller as an
   * attribute, to access it's attributes
   */
  CanTransceiver(const uint8_t node_id, IController &controller);

  /**
   * @brief Sends CAN messages
   */
  bool transmitMessage(utils::io::can::Frame &message) override;

  /**
   * @brief Registers the controller to process incoming CAN messages
   */
  void registerController() override;

  /**
   * @brief This function processes incoming CAN messages
   */
  void processNewData(utils::io::can::Frame &message) override;

  /**
   * @brief If this function returns true, the CAN message is ment for this CAN node
   */
  bool hasId(uint32_t id, bool extended) override;

 private:
  utils::Logger log_;
  uint8_t node_id_;
  IController &controller_;

  static constexpr uint32_t kEmgyTransmit = 0x80;
  static constexpr uint32_t kSdoTransmit  = 0x580;
  static constexpr uint32_t kNmtTransmit  = 0x700;
  static constexpr uint64_t kTimeout      = 70000;  // us
};

}  // namespace hyped::propulsion
