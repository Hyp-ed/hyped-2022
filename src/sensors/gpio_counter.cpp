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
  uint8_t val                     = thepin.wait();     // Ignore first reading
  stripe_counter_.count.value     = 0;
  stripe_counter_.count.timestamp = utils::Timer::getTimeMicros();

  while (sys_.running_) {
    val = thepin.wait();
    if (val == 1) {
      stripe_counter_.count.value = stripe_counter_.count.value + 1;
      log_.DBG3("TEST-KEYENCE", "Stripe Count: %d", stripe_counter_.count.value);
      stripe_counter_.count.timestamp = utils::Timer::getTimeMicros();
      stripe_counter_.operational     = true;
    }
  }
}

void GpioCounter::getData(data::StripeCounter &stripe_counter)
{
  stripe_counter = stripe_counter_;
}

bool GpioCounter::isOnline()
{
  return stripe_counter_.operational;
}
}  // namespace hyped::sensors
