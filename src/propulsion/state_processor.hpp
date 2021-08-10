#pragma once

#include "controller.hpp"

#include "RPM_regulator.hpp"
#include "controller_interface.hpp"
#include "fake_controller.hpp"
#include "state_processor_interface.hpp"
#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>
#include <utils/timer.hpp>

namespace hyped {

namespace motor_control {

using data::Batteries;
using data::Data;
using data::Motors;
using data::Navigation;
using utils::Logger;
using utils::System;
using utils::Timer;

class StateProcessor : public StateProcessorInterface {
 public:
  /**
   * @brief {Initializes the state processors with the amount of motors and the logger}
   * */
  StateProcessor(int motorAmount, Logger &log);

  /**
   * @brief { Sends the desired settings to the motors }
   */
  void initMotors() override;

  /**
   * @brief { Changes the state of the motor controller to preOperational }
   */
  void enterPreOperational() override;

  /**
   * @brief { Stops all motors }
   */
  void quickStopAll() override;

  /**
   * @brief { Checks the motor controller's health }
   */
  void healthCheck() override;

  /**
   * @brief { Checks if the motor controller's error registers }
   */
  bool getFailure() override;

  /**
   * @brief { Tells the controllers to start accelerating the motors }
   */
  void accelerate() override;

  /**
   * @brief { Returns if the motors are initialised already }
   */
  bool isInitialized() override;

  /**
   * @brief Exits the tube with low velocity
   * */
  void servicePropulsion() override;

  /**
   * @brief Returns if a critical error ocurred
   * */
  bool isCriticalFailure() override;

  /**
   * @brief sends the enter operational command
   */
  void sendOperationalCommand();

 protected:
  /**
   * @brief { Registers the controllers to handle CAN transmissions }
   */
  void registerControllers() override;

  /**
   * @brief { Configures the controllers }
   */
  void configureControllers() override;

  /**
   * @brief { Send settings data to the motors }
   */
  void prepareMotors() override;

  /**
   * @brief Calculate the Average rpm of all motors
   *
   * @param controllers
   * @return int32_t
   */
  int32_t calcAverageRPM(ControllerInterface **controllers);

  /**
   * @brief calculate the max Current drawn out of all the motors
   *
   * @param controllers
   * @return int32_t
   */
  int16_t calcMaxCurrent();

  /**
   * @brief Calculate the max temperature out of all the motors
   *
   * @param controllers
   * @return int32_t
   */
  int32_t calcMaxTemp(ControllerInterface **controllers);

  bool useFakeController;
  Logger &log_;
  System &sys_;
  Data &data_;
  Motors motor_data_;
  int motorAmount;
  bool initialized;
  bool criticalError;
  int32_t servicePropulsionSpeed;
  float speed;
  ControllerInterface **controllers;
  RPM_Regulator regulator;
  float velocity;
  Navigation navigationData;
  uint64_t accelerationTimestamp;
  Timer accelerationTimer;
};

}  // namespace motor_control
}  // namespace hyped
