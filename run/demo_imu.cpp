/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description: demo for single imu
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

#include "sensors/imu.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;
using hyped::sensors::Imu;


int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, -1);

  bool is_fifo = false;     // replace with config parse

  Imu imu0(log, 20, is_fifo);

  ImuData data0;
  for (int i = 0; i < 50; i++) {
    imu0.getData(&data0);
    if (is_fifo) {
      for (int i = 0; i < ImuData::kFifoSize; i++) {
        log.INFO("TEST-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2",
                  0,
                  data0.fifo[i][0],
                  data0.fifo[i][1],
                  data0.fifo[i][2]);
      }
    } else {
      log.INFO("TEST-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2",
                0,
                data0.acc[0],
                data0.acc[1],
                data0.acc[2]);
      Thread::sleep(100);
    }
    Thread::sleep(100);
  }
 	return 0;
}
