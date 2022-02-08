#include "gpio_counter.hpp"

#include <stdio.h>

#include <utils/io/gpio.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

GpioCounter::GpioCounter(const std::uint32_t pin)
    : utils::concurrent::Thread(
      utils::Logger("GPIO-COUNTER", utils::System::getSystem().config_.log_level_sensors)),
      pin_(pin),
      sys_(utils::System::getSystem())
{
}

void GpioCounter::run()
{
  utils::io::GPIO thepin(pin_, utils::io::GPIO::Direction::kIn);  // exports pin
  uint8_t val             = thepin.wait();                        // Ignore first reading
  counter_data_.value     = 0;
  counter_data_.timestamp = utils::Timer::getTimeMicros();

  while (sys_.isRunning()) {
    val = thepin.wait();
    if (val == 1) {
      counter_data_.value = counter_data_.value + 1;
      log_.debug("Stripe Count: %d", counter_data_.value);
      counter_data_.timestamp   = utils::Timer::getTimeMicros();
      counter_data_.operational = true;
    }
  }
}

data::CounterData GpioCounter::getData()
{
  return counter_data_;
}

bool GpioCounter::isOnline()
{
  return counter_data_.operational;
}
}  // namespace hyped::sensors
