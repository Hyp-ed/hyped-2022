/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 1/6/19
 * Description: Demo for TMP35 Thermistor using temp_manager
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

#include "sensors/temperature.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"

using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::sensors::Temperature;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, -1);

  Temperature temp_(log, 0);
  Thread::sleep(500);
  log.INFO("TEST-Temp", "Temp instance successfully created");
  for (int i = 0; i < 50; i++) {
    temp_.run();
    int temperature = temp_.getData();
    log.INFO("TEST-Temp", "Thermistor reading: %d degrees C", temperature);
    Thread::sleep(100);
  }
 	return 0;
}
