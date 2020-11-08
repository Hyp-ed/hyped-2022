/*
 * Author:Kornelija Sukyte
 * Organisation: HYPED
 * Date:
 * Description:
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
  // constructing state objects
  idling_          = new Idling(log_, this);
  calibrating_     = new Calibrating(log_, this);
  ready_           = new Ready(log_, this);
  accelerating_    = new Accelerating(log_, this);
  nominal_braking_ = new NominalBraking(log_, this);
  finished_        = new Finished(log_, this);
  failure_braking_ = new FailureBraking(log_, this);
  failure_stopped_ = new FailureStopped(log_, this);


  current_state_ = idling_;  // set current state to point to Idle
}

/**
 *  @brief  Runs state machine thread.
 */
void Main::run()
{
  utils::System &sys = utils::System::getSystem();
  data::Data &data   = data::Data::getInstance();

  data::StateMachine sm_data = data.getStateMachineData();
  sm_data.current_state      = data::State::kReady;  // set current state in data structure
  data.setStateMachineData(sm_data);

  while (sys.running_) {
    current_state_->checkEmergencyStop();
    current_state_->transitionCheck();
  }

  sm_data = data.getStateMachineData();
  log_.INFO("STM", "Exiting. Current state: %s", data::states[sm_data.current_state]);
}

}  // namespace state_machine
}  // namespace hyped
