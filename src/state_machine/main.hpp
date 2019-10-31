/*
 * Author: Ragnor Comerford, Calum McMeekin, Sneha Sinha, Siu Wang (Ian) Ma and Kornelija Sukyte
 * Organisation: HYPED
 * Date:
 * Description:
 * Main instantiates HypedMachine. It also monitors other data and generates Events
 * for the HypedMachine. Note, StateMachine structure in Data is not updated here but
 * in HypedMachine.
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

#ifndef STATE_MACHINE_MAIN_HPP_
#define STATE_MACHINE_MAIN_HPP_

#include <cstdint>
#include "utils/concurrent/thread.hpp"
#include "state_machine/hyped-machine.hpp"
#include "data/data.hpp"
#include "utils/system.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::Logger;
using data::ModuleStatus;

namespace state_machine {

class Main: public Thread {
 public:
  explicit Main(uint8_t id, Logger& log);
  void run() override;

 private:
  HypedMachine hypedMachine;
  utils::System&  sys_;

  // return true iff the event has been fired
  bool checkInitialised();
  bool checkSystemsChecked();
  bool checkReset();
  bool checkOnStart();
  bool checkTelemetryCriticalFailure();
  bool checkCriticalFailure();
  bool checkMaxDistanceReached();
  bool checkOnExit();
  bool checkFinish();
  bool checkAtRest();
  bool checkTimer();

  uint64_t time_start_;
  uint64_t timeout_;

  data::Data&           data_;
  data::Telemetry       telemetry_data_;
  data::Navigation      nav_data_;
  data::StateMachine    sm_data_;
  data::Motors          motor_data_;
  data::Batteries       batteries_data_;
  data::Sensors         sensors_data_;
  data::EmergencyBrakes emergency_brakes_data_;
};

}}      // namespace hyped::motor_control

#endif  // STATE_MACHINE_MAIN_HPP_
