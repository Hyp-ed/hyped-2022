#pragma once

#include "interface.hpp"

#include <cstdint>

#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::sensors {

class GpioCounter : public IGpio, public utils::concurrent::Thread {
 public:
  GpioCounter(utils::Logger &log, int pin);
  ~GpioCounter() {}
  void getData(data::CounterData &counter_data) override;
  bool isOnline() override;
  void run() override;

 private:
  int pin_;
  utils::System &sys_;
  utils::Logger &log_;
  data::CounterData counter_data_;
};
}  // namespace hyped::sensors
