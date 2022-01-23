#pragma once

#include "controller_interface.hpp"
#include "rpm_regulator.hpp"

#include <array>
#include <memory>

#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::propulsion {

class StateProcessor {
 public:
  /**
   * @brief Initializes the state processors with the amount of motors and the logger
   * */
  StateProcessor(utils::Logger &log);

  /**
   * @brief Sends the desired settings to the motors
   */
  void initialiseMotors();

  /**
   * @brief Changes the state of the motor controller to preOperational
   */
  void enterPreOperational();

  /**
   * @brief Stops all motors
   */
  void quickStopAll();

  /**
   * @brief Checks the motor controller's health
   */
  void healthCheck();

  /**
   * @brief Checks if the motor controller's error registers
   */
  bool hasControllerFailure();

  /**
   * @brief Tells the controllers to start accelerating the motors
   */
  void accelerate();

  bool isOverLimits();

  /**
   * @brief Returns if the motors are initialised already
   */
  bool isInitialised();

  /**
   * @brief sends the enter operational command
   */
  void sendOperationalCommand();

 private:
  static constexpr int kMaximumTemperature = 150;
  static constexpr int kMaximumCurrent     = 1500;  // mA
  /**
   * @brief Registers the controllers to handle CAN transmissions
   */
  void registerControllers();

  /**
   * @brief Configures the controllers
   */
  void configureControllers();

  /**
   * @brief Send settings data to the motors
   */
  void prepareMotors();

  /**
   * @brief Calculate the Average rpm of all motors
   *
   * @param controllers
   * @return int32_t
   */
  int32_t calculateAverageRpm();

  /**
   * @brief calculate the max Current drawn out of all the motors
   *
   * @param controllers
   * @return int32_t
   */
  int16_t calculateMaximumCurrent();

  /**
   * @brief Calculate the max temperature out of all the motors
   *
   * @param controllers
   * @return int32_t
   */
  int32_t calculateMaximumTemperature();

  utils::Logger &log_;
  utils::System &sys_;
  data::Data &data_;
  bool is_initialised_;
  std::array<std::unique_ptr<ControllerInterface>, data::Motors::kNumMotors> controllers_;
  RpmRegulator rpm_regulator_;
  uint64_t previous_acceleration_time_;
};

}  // namespace hyped::propulsion
