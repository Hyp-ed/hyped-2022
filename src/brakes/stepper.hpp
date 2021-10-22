#pragma once

#include "interface.hpp"

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/io/gpio.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped {

using data::ModuleStatus;
using utils::Logger;
using utils::concurrent::Thread;
using utils::io::GPIO;

namespace brakes {

class Stepper : public StepperInterface {
 public:
  /**
   * @brief Construct a new Stepper object
   * @param log, node id
   */
  Stepper(uint8_t enable_pin, uint8_t button_pin, Logger &log, uint8_t id);

  /**
   * @brief Deconstruct a Stepper object even if behind `StepperInterface *`
   */
  ~Stepper() {}

  /**
   * @brief checks if brake's button is pressed, notes change in the data struct
   */
  void checkHome() override;

  /**
   * @brief sends retract message
   */
  void sendRetract() override;

  /**
   * @brief sends clamp message
   */
  void sendClamp() override;

  /**
   * @brief checks for brake failure during acceleration
   */
  void checkAccFailure() override;

  void checkBrakingFailure() override;

  bool checkClamped() override;

 private:
  utils::Logger &log_;
  data::Data &data_;
  data::EmergencyBrakes brakes_data_;
  GPIO command_pin_;
  GPIO button_;
  uint8_t brake_id_;
  uint8_t is_clamped_;
};

}  // namespace brakes
}  // namespace hyped
