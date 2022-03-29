#pragma once

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/io/gpio.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::brakes {
class IStepper {
 public:
  virtual void checkHome()           = 0;
  virtual void sendRetract()         = 0;
  virtual void sendClamp()           = 0;
  virtual void checkAccFailure()     = 0;
  virtual void checkBrakingFailure() = 0;
  virtual bool checkClamped()        = 0;

  // Explicit virtual deconstructor needs to be declared *and* defined
  virtual ~IStepper() {}
};
class Stepper : public IStepper {
 public:
  /**
   * @brief Construct a new Stepper object
   * @param log, node id
   */
  Stepper(uint8_t enable_pin, uint8_t button_pin, utils::Logger &log, uint8_t id);

  /**
   * @brief Deconstruct a Stepper object even if behind `IStepper *`
   */
  ~Stepper() {}

  /**
   * @brief {checks if brake's button is pressed, notes change in the data struct}
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
  data::Brakes brakes_data_;
  utils::io::GPIO command_pin_;
  utils::io::GPIO button_;
  uint8_t brake_id_;
  uint8_t is_clamped_;
};

}  // namespace hyped::brakes
