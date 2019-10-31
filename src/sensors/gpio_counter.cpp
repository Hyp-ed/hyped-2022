/*
 * Author: Ragnor Comerford and Jack Horsburgh
 * Organisation: HYPED
 * Date: 18/6/18
 * Description:
 *
 *    Copyright 2018 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <stdio.h>
#include "utils/io/gpio.hpp"
#include "utils/timer.hpp"
#include "sensors/gpio_counter.hpp"

namespace hyped {

using data::Data;
using data::StripeCounter;
using utils::io::GPIO;
using hyped::utils::Logger;

namespace sensors {

GpioCounter::GpioCounter(utils::Logger& log, int pin)
     : pin_(pin),
       sys_(utils::System::getSystem()),
       log_(log)
{}

void GpioCounter::run()
{
  GPIO thepin(pin_, utils::io::gpio::kIn);                // exports pin
  uint8_t val = thepin.wait();  // Ignore first reading
  stripe_counter_.count.value = 0;
  stripe_counter_.count.timestamp =  utils::Timer::getTimeMicros();

  while (sys_.running_) {
    val = thepin.wait();
    if (val == 1) {
      stripe_counter_.count.value = stripe_counter_.count.value+1;
      log_.DBG3("TEST-KEYENCE", "Stripe Count: %d", stripe_counter_.count.value);
      stripe_counter_.count.timestamp =  utils::Timer::getTimeMicros();
      stripe_counter_.operational = true;
    }
  }
}

StripeCounter GpioCounter::getStripeCounter()
{
  return stripe_counter_;
}
}}  // namespace hyped::sensors
