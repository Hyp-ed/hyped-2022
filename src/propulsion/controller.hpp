#pragma once

#include "controller_interface.hpp"
#include "messages.hpp"
#include "nucleo_manager.hpp"

#include <atomic>

#include <data/data.hpp>
#include <propulsion/can/can_transceiver.hpp>
#include <utils/io/can.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>
#include <utils/timer.hpp>

namespace hyped::propulsion {

class Controller : public IController {
 public:
  /**
   * @brief Construct a new Controller object
   * @param log
   * @param id
   */
  Controller(utils::Logger &log, uint8_t id);
  /**
   * @brief Registers controller to recieve and transmit CAN messages.
   */
  void registerController() override;
  /**
   * @brief Apply configuration settings.
   *        (sends 16 can messages to configure the motors correctly)
   */
  void configure() override;
  /**
   * @brief Check for errors or warnings, then enter operational state.
   */
  void enterOperational() override;
  /**
   * @brief Enter preoperational state.
   *
   */
  void enterPreOperational() override;
  /**
   * @brief Check controller state.
   */
  void checkState() override;
  /**
   * @brief Send the target velocity to the motor controller.
   * @param target_velocity - in rpm (calculated in speed calculator)
   */
  void sendTargetVelocity(int32_t target_velocity) override;

  /**
   * @brief Send the target torque to the motor controller.
   *
   * @param target_torque
   */
  void sendTargetTorque(int16_t target_torque);
  /**
   * @brief Send a request to the motor controller to get the actual velocity.
   *
   */
  void updateActualVelocity() override;
  /**
   * @brief Send a request to the motor controller to get the actual velocity
   */
  void updateActualTorque();
  /**
   * @return int32_t - actual velocity of the motor
   */

  int32_t getVelocity() override;
  /**
   * @return int16_t - actual torque of the motor
   */
  int16_t getTorque();
  /**
   * @brief Set the controller to quickstop mode.
   */
  void quickStop() override;
  /**
   * @brief Check the error/warning regesters of the controller.
   */
  void healthCheck() override;
  /**
   * @return critical_failure_
   */
  bool getFailure() override;
  /**
   * @return state_
   */
  ControllerState getControllerState() override;
  /**
   * @return node_id_
   */
  uint8_t getNode_id();
  /**
   * @brief set critical_failure_
   */
  void setFailure(bool failure);
  /**
   * @brief Request the motor temperature from the controller
   */
  void updateMotorTemp() override;
  /**
   * @brief Request the controller temperature from the controller
   */
  void updateControllerTemp();
  /**
   * @return uint8_t - actual temperature of the motor
   */
  uint8_t getMotorTemp() override;
  /**
   * @return uint8_t - actual temperature of the controller
   */
  uint8_t getControllerTemp();
  /**
   * @brief to be called by processNewData if Emergency message is detected.
   * @param message CAN message to process
   */
  void processEmergencyMessage(utils::io::can::Frame &message) override;
  /**
   * @brief Parses error message to find the problem
   * @param error_message
   */
  void processErrorMessage(uint16_t error_message) override;
  /**
   * @brief Called by processNewData if SDO message is detected
   * @param message
   */
  void processSdoMessage(utils::io::can::Frame &message) override;
  /**
   * @brief Called by processNewData if NMT message is detected
   * @param message
   */
  void processNmtMessage(utils::io::can::Frame &message) override;
  /*
   * @brief { Sends state transition message to controller, leaving sufficient time for
   *          controller to change state. If state does not change, throw critical failure }
   *
   * @param[in] { CAN message to be sent, Controller state requested}
   */
  void requestStateTransition(utils::io::can::Frame &message, ControllerState state) override;
  /**
   * @brief Sets the mode of operation to the auto align motor positon mode.
   *        The motor should spin briefly in both directions. This is a testing state and the
   *        rpm is not controllable.
   */
  void autoAlignMotorPosition();

 private:
  /**
   * @brief compact function to call the can sender class with a message,
   *        while checking for critical failure.
   * @param message_template
   * @param len
   */
  bool sendControllerMessage(ControllerMessage message_template);
  /*
   * @brief Sends a CAN frame but waits for a reply
   */
  void sendSdoMessage(utils::io::can::Frame &message);
  /**
   * @brief set critical failure flag to true and write failure to data structure.
   */
  void throwCriticalFailure();

  utils::Logger &log_;
  data::Data &data_;
  data::Motors motor_data_;
  std::atomic<ControllerState> state_;
  uint8_t node_id_;
  std::atomic<bool> critical_failure_;
  std::atomic<int32_t> actual_velocity_;
  std::atomic<int16_t> actual_torque_;
  std::atomic<uint8_t> motor_temperature_;
  std::atomic<uint8_t> controller_temperature_;
  CanSender transceiver_;
  utils::io::can::Frame sdo_message_;
  utils::io::can::Frame nmt_message_;

  // Network management CAN commands:
  const uint8_t kNmtOperational = 0x01;
};
}  // namespace hyped::propulsion
