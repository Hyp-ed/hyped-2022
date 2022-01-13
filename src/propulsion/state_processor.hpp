#pragma once

#include "controller_interface.hpp"
#include "rpm_regulator.hpp"
#include "state_processor_interface.hpp"

#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::propulsion {

class StateProcessor : public StateProcessorInterface {
 public:
  /**
   * @brief Initializes the state processors with the amount of motors and the logger
   * */
  StateProcessor(int motorAmount, utils::Logger &log);

  /**
   * @brief Sends the desired settings to the motors
   */
  void initialiseMotors() override;

  /**
   * @brief Changes the state of the motor controller to preOperational
   */
  void enterPreOperational() override;

  /**
   * @brief Stops all motors
   */
  void quickStopAll() override;

  /**
   * @brief Checks the motor controller's health
   */
  void healthCheck() override;

  /**
   * @brief Checks if the motor controller's error registers
   */
  bool getFailure() override;

  /**
   * @brief Tells the controllers to start accelerating the motors
   */
  void accelerate() override;

  /**
   * @brief Returns if the motors are initialised already
   */
  bool isInitialized() override;

  /**
   * @brief Returns if a critical error ocurred
   */
  bool isCriticalFailure() override;

  /**
   * @brief sends the enter operational command
   */
  void sendOperationalCommand();

 protected:
  /**
   * @brief Registers the controllers to handle CAN transmissions
   */
  void registerControllers() override;

  /**
   * @brief Configures the controllers
   */
  void configureControllers() override;

  /**
   * @brief Send settings data to the motors
   */
  void prepareMotors() override;

  /**
   * @brief Calculate the Average rpm of all motors
   *
   * @param controllers
   * @return int32_t
   */
  int32_t calculateAverageRpm(ControllerInterface **controllers);

  /**
   * @brief calculate the max Current drawn out of all the motors
   *
   * @param controllers
   * @return int32_t
   */
  int16_t calculateMaxCurrent();

  /**
   * @brief Calculate the max temperature out of all the motors
   *
   * @param controllers
   * @return int32_t
   */
  int32_t calculateMaxTemp(ControllerInterface **controllers);

  bool useFakeController;
  utils::Logger &log_;
  utils::System &sys_;
  data::Data &data_;
  data::Motors motor_data_;
  int motorAmount;
  bool initialized;
  bool criticalError;
  int32_t servicePropulsionSpeed;
  float speed;
  ControllerInterface **controllers;
  RpmRegulator regulator;
  float velocity;
  data::Navigation navigationData;
  uint64_t accelerationTimestamp;
  utils::Timer accelerationTimer;
};

}  // namespace hyped::propulsion
