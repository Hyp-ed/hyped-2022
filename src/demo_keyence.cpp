/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 24/5/18
 * Description: Test keyence with GpioCounter, use for basic functionality test and
 *  statistical test for sensor accuracy
 *
 *    Copyright 2019 HYPED
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

#include "utils/concurrent/thread.hpp"
#include "utils/io/gpio.hpp"
#include "sensors/gpio_counter.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"
#include "utils/system.hpp"

using hyped::utils::concurrent::Thread;
using hyped::utils::io::GPIO;
using hyped::sensors::GpioCounter;
using hyped::utils::Logger;
using hyped::utils::Timer;
using hyped::data::StripeCounter;

constexpr uint32_t kStripeNum = 500;          // change value depending on configuration of run

int main(int argc, char* argv[]) {
    hyped::utils::System::parseArgs(argc, argv);
    Logger log(true, 1);
    GpioCounter keyence(log, 69);

    Thread::sleep(500);

    // keyence.run();
    keyence.start();
    // keyence.sleep(50);
    StripeCounter stripe_data = keyence.getStripeCounter();
    log.INFO("TEST-KEYENCE", "Start time: %d", stripe_data.count.timestamp);
    log.INFO("TEST-KEYENCE", "Start count: %d", stripe_data.count.value);
    uint32_t stripe_count = 0;
    while (stripe_count < kStripeNum) {
      stripe_data = keyence.getStripeCounter();
      if (stripe_data.count.value > stripe_count) {
        stripe_count = stripe_data.count.value;
        log.INFO("TEST-KEYENCE","Stripe Count: %d",stripe_count);
      }
      Thread::sleep(50);                       // remove if want to see output quickly
    }
    int64_t time = stripe_data.count.timestamp;

    log.INFO("TEST-KEYENCE", "Final stripe count = %d. Final timestamp = %d", stripe_count, time);
}