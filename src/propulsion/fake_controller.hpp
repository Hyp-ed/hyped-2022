#pragma once

#include "controller_interface.hpp"

#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/timer.hpp>

namespace hyped::propulsion {

class FakeController : public IController {
 public:
  /**
   * @brief  Construct a new Fake Controller object
   */
  FakeController(utils::Logger &log, const uint8_t id, const bool is_faulty);
  /**
   * @brief  Registers controller to recieve and transmit CAN messages.
   *         note: empty implementation.
   */
  void registerController() override;
  /**
   * @brief  Applies configuration settings
   */
  void configure() override;
  /**
   * @brief  Check for errors or warnings, then enter operational state.
   */
  void enterOperational() override;
  /**
   * @brief  Enter preoperational state.
   */
  void enterPreOperational() override;
  /**
   * @brief  check controller state.
   */
  void checkState() override;
  /**
   * @brief  Sets actual velocity = target velocity
   * @param[in]  target_velocity in rpm (Calculated in speed calculator).
   */
  void sendTargetVelocity(const int32_t target_velocity) override;
  /**
   * @brief  Send a request to the motor controller to get the actual velocity.
   *         note: empty implementation.
   */
  void updateActualVelocity() override;
  /**
   * @return int32_t - actual velocity of the motor
   */
  int32_t getVelocity() override;
  /**
   * @brief  Sets the controller to quickstop mode.
   */
  void quickStop() override;
  /**
   * @brief  if isFaulty is set to true then after a random amount of time between 3 and 23 seconds
   *         critical_failure_ will be set to true.
   *         if isFaulty is set to false then the controller will operate as normal.
   */
  void healthCheck() override;
  /**
   * @return the failure flag, critical_failure_.
   */
  bool getFailure() override;
  /**
   * @brief Get and return the state of the controller.
   * @return state_
   */
  ControllerState getControllerState() override;

  uint8_t getMotorTemp() override;

  // empty functions from interface not used in the fake controller
  void processEmergencyMessage(utils::io::can::Frame &) override
  { /*EMPTY*/
  }
  void processErrorMessage(uint16_t) override
  { /*EMPTY*/
  }
  void processSdoMessage(utils::io::can::Frame &) override
  { /*EMPTY*/
  }
  void processNmtMessage(utils::io::can::Frame &) override
  { /*EMPTY*/
  }
  void requestStateTransition(utils::io::can::Frame &, ControllerState) override
  { /*EMPTY*/
  }
  void updateMotorTemp() override
  { /*EMPTY*/
  }

 private:
  /**
   * @brief Times the duration of the run. Starts when we enter the accelerating state.
   */
  void startTimer();
  utils::Logger &log_;
  data::Data &data_;
  ControllerState state_;
  utils::Timer timer_;
  uint8_t id_;
  bool is_faulty_;
  bool critical_failure_;
  int32_t actual_velocity_;
  uint64_t start_time_;
  bool timer_started_;
  uint64_t fail_time_;
  uint8_t motor_temperature_;
};

}  // namespace hyped::propulsion
