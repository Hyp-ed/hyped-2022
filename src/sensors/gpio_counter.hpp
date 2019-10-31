/*
 * Author: Ragnor Comerford and Jack Horsburgh
 * Organisation: HYPED
 * Date: 19/06/18
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

#ifndef SENSORS_GPIO_COUNTER_HPP_
#define SENSORS_GPIO_COUNTER_HPP_

#include <cstdint>

#include "sensors/interface.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"

namespace hyped {

using utils::Logger;
using utils::concurrent::Thread;
using utils::System;

namespace sensors {

class GpioCounter: public GpioInterface, public Thread {            // interface.hpp
 public:
  GpioCounter(utils::Logger& log, int pin);
  ~GpioCounter() {}
  data::StripeCounter getStripeCounter() override;      // data.hpp, data.cpp<array> data_point.hpp
  void run() override;

 private:
  int pin_;
  System&  sys_;
  utils::Logger& log_;
  data::StripeCounter stripe_counter_;
};
}}  // namespace hyped::sensors

#endif  // SENSORS_GPIO_COUNTER_HPP_
