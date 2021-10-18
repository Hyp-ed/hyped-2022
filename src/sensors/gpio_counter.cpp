#include "gpio_counter.hpp"

#include <stdio.h>

//Where to find the utils??
#include <utils/io/gpio.hpp>
#include <utils/timer.hpp>

namespace hyped {

using data::Data;
using data::StripeCounter;
using hyped::utils::Logger;
using utils::io::GPIO;

namespace sensors {

GpioCounter::GpioCounter(utils::Logger &log, int pin)
    : pin_(pin),
      sys_(utils::System::getSystem()),
      log_(log)
{
}

void GpioCounter::run()
{
  GPIO thepin(pin_, utils::io::gpio::kIn);          // exports pin
  uint8_t val                     = thepin.wait();  // Ignore first reading
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

//Is this the method where the count is updated?? What does the operational part do?
void GpioCounter::getData(StripeCounter *stripe_counter)
{
  stripe_counter->count       = stripe_counter_.count;
  stripe_counter->operational = stripe_counter_.operational;
}

bool GpioCounter::isOnline()
{
  return stripe_counter_.operational;
}
}  // namespace sensors
}  // namespace hyped
