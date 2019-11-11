/*
* Author: Kornelija Sukyte
* Organisation: HYPED
* Date:
* Description: Entrypoint class to the embrake module, started in it's own thread.
*
*    Copyright 2019 HYPED
*    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
*    except in compliance with the License. You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
*    Unless required by applicable law or agreed to in writing, software distributed under
*    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
*    either express or implied. See the License for the specific language governing permissions and
*    limitations under the License.
*/

#include "main.hpp"
#include "utils/config.hpp"

namespace hyped {
namespace embrakes {

Main::Main(uint8_t id, Logger &log)
  : Thread(id, log),
    log_(log),
    data_(data::Data::getInstance()),
    sys_(utils::System::getSystem())
{
  // parse GPIO pins from config.txt file
  for (int i = 0; i < 4; i++) {
    command_pins_[i] = sys_.config->embrakes.command[i];
    button_pins_[i] = sys_.config->embrakes.button[i];
  }
  brake_1 = new Stepper(command_pins_[0], button_pins_[0], log_, 1);
  // Stepper brake_2(command_pins_[1], button_pins_[1], log_, 2);
  // Stepper brake_3(command_pins_[2], button_pins_[2], log_, 3);
  // Stepper brake_4(command_pins_[3], button_pins_[3], log_, 4);
}

void Main::run()
{
  log_.INFO("Brakes", "Thread started");

  System &sys = System::getSystem();

  while (sys.running_) {
    // Get the current state of embrakes and state machine modules from data
    em_brakes_ = data_.getEmergencyBrakesData();
    sm_data_ = data_.getStateMachineData();
    tlm_data_ = data_.getTelemetryData();

    switch (sm_data_.current_state) {
      case data::State::kIdle:
        if (!tlm_data_.nominal_braking_command) {
          log_.INFO("Brakes", "RETRACT COMMAND");
          if (brake_1->checkClamped()) {
            brake_1->sendRetract();
          }
          // if(brake_2->checkClamped()){
          //   brake_2->sendRetract();
          // }
          // if(brake_3->checkClamped){
          //   brake_3->sendRetract();
          // }
          // if(brake_4->checkClamped){
          //   brake_4->sendRetract();
          // }
          Thread::sleep(1000);
          brake_1->checkHome();
          // brake_2->checkHome();
          // brake_3->checkHome();
          // brake_4->checkHome();

        } else if (tlm_data_.nominal_braking_command) {
          log_.INFO("Brakes", "ENGAGE COMMAND");
          if (!brake_1->checkClamped()) {
            brake_1->sendClamp();
          }
          // if(!brake_2->checkClamped()){
          //   brake_2->sendClamp();
          // }
          // if(!brake_3->checkClamped){
          //   brake_3->sendClamp();
          // }
          // if(!brake_4->checkClamped){
          //   brake_4->sendClamp();
          // }
          Thread::sleep(1000);
          brake_1->checkHome();
          // brake_2->checkHome();
          // brake_3->checkHome();
          // brake_4->checkHome();
        }
        break;
      case data::State::kCalibrating:
        if (brake_1->checkClamped()) {
          brake_1->sendRetract();
        }
        // if(brake_2->checkClamped()){
        //   brake_2->sendRetract();
        // }
        // if(brake_3->checkClamped()){
        //   brake_3->sendRetract();
        // }
        // if(brake_4->checkClamped()){
        //   brake_4->sendRetract();
        // }
        if (!brake_1->checkClamped()) {
          em_brakes_.module_status = ModuleStatus::kReady;
          data_.setEmergencyBrakesData(em_brakes_);
        }
        Thread::sleep(1000);
        brake_1->checkHome();
        // brake_2->checkHome();
        // brake_3->checkHome();
        // brake_4->checkHome();
        break;
      case data::State::kAccelerating:
        brake_1->checkAccFailure();
        // brake_2->checkAccFailure();
        // brake_3->checkAccFailure();
        // brake_4->checkAccFailure();
        break;
      case data::State::kNominalBraking:
        if (!brake_1->checkClamped()) {
          brake_1->sendClamp();
        }
        // if(!brake_2->checkClamped()){
        //   brake_2->sendClamp();
        // }
        // if(!brake_3->checkClamped()){
        //   brake_3->sendClamp();
        // }
        // if(!brake_4->checkClamped()){
        //   brake_4->sendClamp();
        // }
        Thread::sleep(1000);
        brake_1->checkHome();
        // brake_2->checkHome();
        // brake_3->checkHome();
        // brake_4->checkHome();

        brake_1->checkBrakingFailure();
        // brake_2->checkBrakingFailure();
        // brake_3->checkBrakingFailure();
        // brake_4->checkBrakingFailure();
        break;
      case data::State::kFinished:
        if (tlm_data_.nominal_braking_command) {
          if (brake_1->checkClamped()) {
            brake_1->sendRetract();
          }
          // if(brake_2->checkClamped()){
          //   brake_2->sendRetract();
          // }
          // if(brake_3->checkClamped){
          //   brake_3->sendRetract();
          // }
          // if(brake_4->checkClamped){
          //   brake_4->sendRetract();
          // }
          Thread::sleep(1000);
          brake_1->checkHome();
          // brake_2->checkHome();
          // brake_3->checkHome();
          // brake_4->checkHome();
        } else if (!tlm_data_.nominal_braking_command) {
          if (!brake_1->checkClamped()) {
            brake_1->sendClamp();
          }
          // if(!brake_2->checkClamped()){
          //   brake_2->sendClamp();
          // }
          // if(!brake_3->checkClamped){
          //   brake_3->sendClamp();
          // }
          // if(!brake_4->checkClamped){
          //   brake_4->sendClamp();
          // }
          Thread::sleep(500);
          brake_1->checkHome();
          // brake_2->checkHome();
          // brake_3->checkHome();
          // brake_4->checkHome();
        }
        break;
      default:
        break;
    }
  }
  log_.INFO("Brakes", "Thread shutting down");
}

}  // namespace embrakes
}  // namespace hyped
