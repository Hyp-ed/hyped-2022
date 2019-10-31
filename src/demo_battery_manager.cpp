/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 3/4/19
 * Description:
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

#include "sensors/bms_manager.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::sensors::BmsManager;
using namespace hyped::data;
using namespace std;
using hyped::sensors::BmsManager;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, -1);
  Data& data_ = Data::getInstance();
  BmsManager bms_manager(log);
  bms_manager.start();
  Thread::sleep(500);
  StateMachine state_machine = data_.getStateMachineData();
  state_machine.current_state = State::kReady;
  data_.setStateMachineData(state_machine);
  Batteries batteries = data_.getBatteriesData();

  // change to accel state

  log.INFO("TEST-Bms", "Bms instance successfully created");
  for (int i = 0; i < 500; i++) {
    batteries = data_.getBatteriesData();
    auto lp0 = batteries.low_power_batteries[0];
    auto lp1 = batteries.low_power_batteries[1];
    auto lp2 = batteries.low_power_batteries[2];
    // auto hp = batteries.high_power_batteries[0];
    log.INFO("TEST-BMSLP", "LP0: V = %udV, C = %ddA, avg_temp  = %dC, charge = %d",
        lp0.voltage, lp0.current, lp0.average_temperature, lp0.charge);
     log.INFO("TEST-BMSLP", "LP1: V = %udV, C = %ddA, avg_temp  = %dC, charge = %d",
        lp1.voltage, lp1.current, lp1.average_temperature, lp1.charge);
     log.INFO("TEST-BMSLP", "LP2: V = %udV, C = %ddA, avg_temp  = %dC, charge = %d",
        lp2.voltage, lp2.current, lp2.average_temperature, lp2.charge);
    // log.INFO("TEST-BMSHP", "V = %udV, C = %udA, low_temp  = %dC, avg_temp  = %dC, high_temp  = %dC, charge = %d, hi_volt_cell = %umV, lo_volt_cell = %umV", // NOLINT[whitespace/line_length]
    //     hp.voltage, hp.current, hp.low_temperature, hp.average_temperature, hp.high_temperature, hp.charge, hp.high_voltage_cell, hp.low_voltage_cell); // NOLINT[whitespace/line_length]
    Thread::sleep(500);
  }
 	return 0;
}