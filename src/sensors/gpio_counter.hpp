#pragma once

#include "interface.hpp"

#include <cstdint>

#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::sensors {

class GpioCounter : public ICounter, public utils::concurrent::Thread {
 public:
  GpioCounter(std::uint32_t pin);
  ~GpioCounter() {}
  data::CounterData getData() override;
  bool isOnline() override;
  void run() override;

 private:
  int pin_;
  utils::System &sys_;
  data::CounterData counter_data_;
};
}  // namespace hyped::sensors
