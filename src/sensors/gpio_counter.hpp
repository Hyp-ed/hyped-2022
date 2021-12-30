#pragma once

#include "interface.hpp"

#include <cstdint>

#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::sensors {

class GpioCounter : public GpioInterface, public utils::concurrent::Thread {  // interface.hpp
 public:
  GpioCounter(utils::Logger &log, int pin);
  ~GpioCounter() {}
  void getData(data::StripeCounter &stripe_counter) override;
  bool isOnline() override;
  void run() override;

 private:
  int pin_;
  utils::System &sys_;
  utils::Logger &log_;
  data::StripeCounter stripe_counter_;
};
}  // namespace hyped::sensors
