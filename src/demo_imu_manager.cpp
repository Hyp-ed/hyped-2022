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

#include "sensors/imu_manager.hpp"
#include "sensors/imu.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

#define MANAGER 1

using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;
using namespace std;
using hyped::sensors::ImuManager;
#if !MANAGER
using hyped::data::ImuData;
using hyped::sensors::Imu;
#endif

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, -1);

  #if MANAGER
  DataPoint<array<ImuData, Sensors::kNumImus>> data_array_;
  Data& data_ = Data::getInstance();
  ImuManager imu_manager_(log);
  imu_manager_.start();
  Thread::sleep(500);
  
  #else
  Imu imu0(log, 20, 0x08);
  Imu imu1(log, 7, 0x08);
  Imu imu2(log, 112, 0x08);
  Imu imu3(log, 111, 0x08);

  ImuData data0;
  ImuData data1;
  ImuData data2;
  ImuData data3;

  for (int i = 0; i<50; i++) {
    imu0.getData(&data0);
    log.INFO("TEST-ImuManager", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 0, data0.acc[0], data0.acc[1], data0.acc[2]);
    imu1.getData(&data1);
    log.INFO("TEST-ImuManager", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 1, data1.acc[0], data1.acc[1], data1.acc[2]);
    imu2.getData(&data2);
    log.INFO("TEST-ImuManager", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 2, data2.acc[0], data2.acc[1], data2.acc[2]);
    imu3.getData(&data3);
    Thread::sleep(100);
  }
  #endif

  StateMachine state_machine = data_.getStateMachineData();
  state_machine.current_state = State::kAccelerating;   // change state for different accel values
  data_.setStateMachineData(state_machine);

  log.INFO("TEST-ImuManager", "Imu instance successfully created");
  for (int i = 0; i < 50; i++) {
    data_array_ = data_.getSensorsImuData();
    Thread::sleep(100);
    for (int j = 0; j < Sensors::kNumImus; j++) {
      log.INFO("TEST-ImuManager", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", j, data_array_.value[j].acc[0], data_array_.value[j].acc[1], data_array_.value[j].acc[2]);
    }
  }
 	return 0;
}