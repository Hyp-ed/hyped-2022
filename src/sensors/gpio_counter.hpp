#pragma once

#include "interface.hpp"

#include <cstdint>

#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::sensors {

class GpioCounter : public ICounter, public utils::concurrent::Thread {
 public:
  GpioCounter(utils::Logger &log, int pin);
  ~GpioCounter() {}
  data::CounterData getData() override;
  bool isOnline() override;
  void run() override;

 private:
  int pin_;
  utils::System &sys_;
  utils::Logger &log_;
  data::CounterData counter_data_;
};
}  // namespace hyped::sensors
