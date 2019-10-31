/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description: Demo for ADC Pins on BeagleBone Black
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
#include "utils/io/adc.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"

using hyped::utils::io::ADC;
using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::Timer;
using hyped::utils::concurrent::Thread;
namespace io = hyped::utils::io;

int main(int argc, char* argv[]) { 
  System::parseArgs(argc, argv);
  Logger log(true, -1);

  ADC analog(0, log);
  Thread::sleep(100);

  while(1) {
    log.INFO("DEMO-ADC", "Analog in value: %d", analog.read());   // digital range [0,4095]
    Thread::sleep(100);
  }
}
