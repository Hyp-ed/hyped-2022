#pragma once

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/io/gpio.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::brakes {
class IBrake {
 public:
  virtual void retract()   = 0;
  virtual void engage()    = 0;
  virtual bool isEngaged() = 0;

  // Explicit virtual deconstructor needs to be declared *and* defined
  virtual ~IBrake() {}
};
class Brake : public IBrake {
 public:
  /**
   * @brief Construct a new Brake object
   * @param log, node id
   */
  Brake(const uint8_t enable_pin, const uint8_t button_pin, const uint8_t id);

  /**
   * @brief Deconstruct a Brake object even if behind `IBrake *`
   */
  ~Brake() {}

  /**
   * @brief {checks if brake's button is pressed, notes change in the data struct}
   */

  /**
   * @brief sends retract message
   */
  void retract() override;

  /**
   * @brief sends clamp message
   */
  void engage() override;

  bool isEngaged() override;

 private:
  utils::Logger log_;
  utils::io::Gpio command_pin_;
  utils::io::Gpio button_;
  const uint8_t id_;
};

}  // namespace hyped::brakes
