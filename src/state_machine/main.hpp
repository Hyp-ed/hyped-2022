/*
 * Author: Kornelija Sukyte
 * Organisation: HYPED
 * Date:
 * Description:
 * Main instantiates HypedMachine. It also monitors other data and generates Events
 * for the HypedMachine. Note, StateMachine structure in Data is not updated here but
 * in HypedMachine.
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

#ifndef STATE_MACHINE_MAIN_HPP_
#define STATE_MACHINE_MAIN_HPP_

#include <cstdint>
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
#include "utils/system.hpp"
#include "state_machine/state.hpp"
#include "utils/config.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::Logger;
using data::ModuleStatus;

namespace state_machine {

class State;
class Ready;
class Accelerating;
class Braking;
class Finished;
class Main: public Thread {
 public:
  explicit Main(uint8_t id, Logger& log);
  void run() override;

  State          *current_state_;
  Ready          *ready_;
  Accelerating   *accelerating_;
  Braking        *braking_;
  Finished       *finished_;
};

}  // namespace state_machine
}  // namespace hyped

#endif  // STATE_MACHINE_MAIN_HPP_
