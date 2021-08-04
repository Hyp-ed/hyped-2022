/*
 * Author: Kornelija Sukyte, Franz Miltz
 * Organisation: HYPED
 * Date:
 * Description: Implements the behaviour described in main.hpp
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
#include "state_machine/main.hpp"

#include <cstdint>

namespace hyped {
namespace state_machine {

Main::Main(uint8_t id, Logger &log) : Thread(id, log)
{
  current_state_ = Idle::getInstance();  // set current state to point to Idle
}

void Main::run()
{
  utils::System &sys = utils::System::getSystem();
  data::Data &data   = data::Data::getInstance();

  current_state_->enter(log_);

  State *new_state;
  while (sys.running_) {
    // checkTransition returns a new state or nullptr
    if ((new_state = current_state_->checkTransition(log_))) {
      current_state_->exit(log_);
      current_state_ = new_state;
      current_state_->enter(log_);
    }

    // Yielding because running the loop twice without any other thread being active
    // will result in identical behaviour and thus waste resources.
    yield();
  }

  data::StateMachine sm_data = data.getStateMachineData();
  log_.INFO("STM", "exiting. current state: %s", data::states[sm_data.current_state]);
}

}  // namespace state_machine
}  // namespace hyped
