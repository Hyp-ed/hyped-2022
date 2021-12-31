#include "gpio_counter.hpp"

#include <stdio.h>

#include <utils/io/gpio.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

GpioCounter::GpioCounter(utils::Logger &log, int pin)
    : pin_(pin),
      sys_(utils::System::getSystem()),
      log_(log)
{
}

void GpioCounter::run()
{
  utils::io::GPIO thepin(pin_, utils::io::gpio::kIn);  // exports pin
  uint8_t val                   = thepin.wait();       // Ignore first reading
  counter_data_.count.value     = 0;
  counter_data_.count.timestamp = utils::Timer::getTimeMicros();

  while (sys_.running_) {
    val = thepin.wait();
    if (val == 1) {
      counter_data_.count.value = counter_data_.count.value + 1;
      log_.DBG3("TEST-KEYENCE", "Stripe Count: %d", counter_data_.count.value);
      counter_data_.count.timestamp = utils::Timer::getTimeMicros();
      counter_data_.operational     = true;
    }
  }
}

void GpioCounter::getData(data::CounterData &counter_data)
{
  counter_data = counter_data_;
}

bool GpioCounter::isOnline()
{
  return counter_data_.operational;
}
}  // namespace hyped::sensors
