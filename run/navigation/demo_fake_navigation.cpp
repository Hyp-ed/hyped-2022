
/*
 * Author: Lukas Schaefer
 * Co-Author: Justus Rudolph
 * Organisation: HYPED
 * Date: 01/04/19, minor edits 20/02/20
 * Description: Demo for Multiple IMU Navigation with fake data
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

#include "data/data.hpp"
#include "navigation/main.hpp"
#include "sensors/fake_gpio_counter.hpp"
#include "sensors/imu_manager.hpp"
#include "sensors/main.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"

using hyped::data::Data;
using hyped::data::ModuleStatus;
using hyped::data::Sensors;
using hyped::data::State;
using hyped::data::StateMachine;
using hyped::navigation::Main;
using hyped::utils::Logger;
using hyped::utils::System;
using hyped::utils::concurrent::Thread;

int main(int argc, char *argv[])
{
  System::parseArgs(argc, argv);
  System &sys       = System::getSystem();
  static Data &data = Data::getInstance();
  Logger *log_nav   = new Logger(sys.verbose_nav, sys.debug_nav);

  if (sys.stationary_run) {
    log_nav->INFO("NAV", "STATIONARY RUN INITIALISED");
  } else if (sys.outside_run) {
    log_nav->INFO("NAV", "OUTSIDE RUN INITIALISED");
  } else {
    log_nav->INFO("NAV", "OFFICIAL RUN INITIALISED");
  }

  // use all fake sensors
  sys.fake_imu         = true;
  sys.fake_batteries   = true;
  sys.fake_temperature = true;
  sys.fake_embrakes    = true;
  sys.fake_motors      = true;
  sys.fake_keyence     = true;

  // Initialise sensors
  hyped::sensors::Main *sensors_main = new hyped::sensors::Main(1, *log_nav);
  sensors_main->start();
  Main *main = new Main(2, *log_nav);
  main->start();

  log_nav->INFO("MAIN", "Set state to CALIBRATING");
  StateMachine state_machine  = data.getStateMachineData();
  state_machine.current_state = State::kCalibrating;
  data.setStateMachineData(state_machine);

  ModuleStatus nav_state = data.getNavigationData().module_status;
  while (nav_state != ModuleStatus::kReady) {
    nav_state = data.getNavigationData().module_status;
    Thread::sleep(100);
  }

  log_nav->INFO("MAIN", "Set state to ACCELERATING");
  state_machine.current_state = State::kAccelerating;
  data.setStateMachineData(state_machine);

  // Accelerating for 20.25s
  Thread::sleep(20250);

  log_nav->INFO("MAIN", "Set state to NOMINAL BRAKING");
  state_machine.current_state = State::kNominalBraking;
  data.setStateMachineData(state_machine);

  // Breaking for 4s
  Thread::sleep(4000);

  // Exit gracefully
  sys.running_ = false;
  main->join();

  return 0;
}
