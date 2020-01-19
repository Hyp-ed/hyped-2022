/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 3/4/19
 * Description: Demo for MPU9250 sensor using imu_manager
 * Troubleshooting:
 * If a single sensor does not initialise, try reconfiguring the chip_select_ pin.
 *  The GPIO pin may be faulty, so attempt with another pin and change
 *  chip_select_ pin list in imu_manager.cpp accordingly.
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
using namespace std;
using hyped::data::ImuData;
using hyped::sensors::Imu;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger& log = hyped::utils::System::getLogger();
  Imu imu0(log, 20, true);

  ImuData data0;

  for (int i = 0; i<50; i++) {
    imu0.getData(&data0);
    // log.INFO("TEST-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 0, data0.acc[0], data0.acc[1], data0.acc[2]);
    Thread::sleep(100);
  }

 	return 0;
}
