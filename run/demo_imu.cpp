/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 30/1/20
 * Description: Demo for ICM-20948 sensor using imu_manager
 *
 *    Copyright 2020 HYPED
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

#include "sensors/imu_manager.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;
using hyped::data::Sensors;
using hyped::sensors::ImuManager;
using hyped::utils::System;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger& log = hyped::utils::System::getLogger();
  Data& data = Data::getInstance();

  DataPoint<array<ImuData, Sensors::kNumImus>> data_array_;
  ImuManager imu(log);

  StateMachine state_machine = data.getStateMachineData();
  state_machine.current_state = State::kAccelerating;   // change state for different accel values
  data.setStateMachineData(state_machine);

  imu.start();

  while(true) {
    data_array_ = data.getSensorsImuData();
    for (int j = 0; j < 1; j++) {
      log.INFO("TEST-ImuManager", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2",
                j,
                data_array_.value[j].acc[0],
                data_array_.value[j].acc[1],
                data_array_.value[j].acc[2]);
    }
    Thread::sleep(100);
  }

 	return 0;
}
