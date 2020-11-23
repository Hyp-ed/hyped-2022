/*
 * Author: Kornelija Sukyte, Franz Miltz
 * Organisation: HYPED
 * Date:
 * Description: Main is the state machine. This is where all the state is stored and how we interact
 * with the rest of HYPED. The SM only provides a frame work though, the actual logic is implemented
 * in the other files.
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

#include "data/data.hpp"
#include "state_machine/state.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/config.hpp"
#include "utils/system.hpp"

namespace hyped {

using data::ModuleStatus;
using utils::Logger;
using utils::concurrent::Thread;

namespace state_machine {

class State;  // Forward declaration

class Main : public Thread {
 public:
  explicit Main(uint8_t id, Logger &log);

  /**
   *  @brief  Runs state machine thread.
   */
  void run() override;

  /*
   * @brief  Current state of the pod
   */
  State *current_state_;
};

}  // namespace state_machine
}  // namespace hyped

#endif  // STATE_MACHINE_MAIN_HPP_
