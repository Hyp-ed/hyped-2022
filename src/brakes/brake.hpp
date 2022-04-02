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
  Brake(const uint8_t enable_pin, const uint8_t button_pin, const uint8_t id);

  void retract() override;
  void engage() override;
  bool isEngaged() override;

 private:
  utils::Logger log_;
  utils::io::Gpio command_pin_;
  utils::io::Gpio button_;
  const uint8_t id_;
};

}  // namespace hyped::brakes
