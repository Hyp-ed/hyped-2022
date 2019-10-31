/*
 * Author: Iain Macpherson
 * Co-Author: George Karabassis
 * Organisation: HYPED
 * Date: 11/03/2019
 * Description: Main class for the Motor Controller
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef PROPULSION_CONTROLLER_HPP_
#define PROPULSION_CONTROLLER_HPP_

#include <atomic>

#include "propulsion/controller_interface.hpp"
#include "propulsion/file_reader.hpp"
#include "propulsion/can/can_sender.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"
#include "utils/logger.hpp"
#include "utils/io/can.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"


namespace hyped {

using std::atomic;

using utils::concurrent::Thread;
using utils::io::can::Frame;
using utils::Logger;

namespace motor_control {

class Controller : public ControllerInterface {
 public:
  /**
   * @brief Construct a new Controller object
   * @param log
   * @param id
   */
  Controller(Logger& log, uint8_t id);
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
  void processEmergencyMessage(utils::io::can::Frame& message) override;
  /**
   * @brief Parses error message to find the problem
   * @param error_message
   */
  void processErrorMessage(uint16_t error_message) override;
  /**
   * @brief Called by processNewData if SDO message is detected
   * @param message
   */
  void processSdoMessage(utils::io::can::Frame& message) override;
  /**
   * @brief Called by processNewData if NMT message is detected
   * @param message
   */
  void processNmtMessage(utils::io::can::Frame& message) override;
  /*
   * @brief { Sends state transition message to controller, leaving sufficient time for
   *          controller to change state. If state does not change, throw critical failure }
   *
   * @param[in] { CAN message to be sent, Controller state requested}
   */
  void requestStateTransition(utils::io::can::Frame& message, ControllerState state) override;
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
  void sendSdoMessage(utils::io::can::Frame& message);
  /**
   * @brief set critical failure flag to true and write failure to data structure.
   */
  void throwCriticalFailure();

  Logger&                   log_;
  data::Data&               data_;
  data::Motors              motor_data_;
  atomic<ControllerState>   state_;
  uint8_t                   node_id_;
  atomic<bool>              critical_failure_;
  atomic<int32_t>           actual_velocity_;
  atomic<int16_t>           actual_torque_;
  atomic<uint8_t>           motor_temperature_;
  atomic<uint8_t>           controller_temperature_;
  CanSender                 sender;
  Frame             sdo_message_;
  Frame             nmt_message_;

  // Network management CAN commands:
  const uint8_t     kNmtOperational        = 0x01;

  // Paths to the different files of message data.
  const char* kConfigMsgFile = "data/in/controllerConfigFiles/configure.txt";
  const char* kEnterOpMsgFile = "data/in/controllerConfigFiles/enter_operational.txt";
  const char* kEnterPreOpMsgFile = "data/in/controllerConfigFiles/enter_preOperational.txt";
  const char* kCheckStateMsgFile = "data/in/controllerConfigFiles/check_state.txt";
  const char* kSendTargetVelMsgFile = "data/in/controllerConfigFiles/send_target_velocity.txt";
  const char* kSendTargetTorqMsgFile = "data/in/controllerConfigFiles/send_target_torque.txt";
  const char* kUpdateActualVelMsgFile = "data/in/controllerConfigFiles/update_actual_velocity.txt";
  const char* kUpdateActualTorqMsgFile = "data/in/controllerConfigFiles/update_actual_torque.txt";
  const char* kQuickStopMsgFile = "data/in/controllerConfigFiles/quick_stop.txt";
  const char* kHealthCheckMsgFile = "data/in/controllerConfigFiles/health_check.txt";
  const char* kUpdateMotorTempFile = "data/in/controllerConfigFiles/update_motor_temp.txt";
  const char* kUpdateContrTempFile = "data/in/controllerConfigFiles/update_contr_temp.txt";
  const char* kAutoAlignMsgFile = "data/in/controllerConfigFiles/auto_align.txt";

 public:
  // Arrays of messages sent to controller (see config files for details about message contents)
  ControllerMessage configMsgs_[24];
  ControllerMessage enterOpMsgs_[4];
  ControllerMessage enterPreOpMsg_[1];
  ControllerMessage checkStateMsg_[1];
  ControllerMessage sendTargetVelMsg[1];
  ControllerMessage sendTargetTorqMsg[1];
  ControllerMessage updateActualVelMsg[1];
  ControllerMessage updateActualTorqMsg[1];
  ControllerMessage quickStopMsg[1];
  ControllerMessage healthCheckMsgs[2];
  ControllerMessage updateMotorTempMsg[1];
  ControllerMessage updateContrTempMsg[1];
  ControllerMessage autoAlignMsg[1];
};
}}  // namespace hyped::motor_control

#endif  // PROPULSION_CONTROLLER_HPP_
