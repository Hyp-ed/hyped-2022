/*
 * Author: J. Ridley
 * Organisation: HYPED
 * Date: March 2019
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

#include <string>
#include "fake_client.hpp"
#include "utils/system.hpp"
#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped {
namespace telemetry {

FakeClient::FakeClient() 
  : log_(utils::System::getLogger()),
    data(data::Data::getInstance()) {  //how do i get an instance of statemachine data
    log_.INFO("Telemetry", "Fake client object created");
}

bool FakeClient::connect() {
  log_.INFO("Telemetry", "Fake client connected");
  return true;
}

bool FakeClient::sendData(std::string message) {
  return true;
}

std::string FakeClient::receiveData() {
  //Thread.sleep(1000);                              //how do i get sleep working
  
  std::string fake_message;
  switch (data.getStateMachineData().current_state) { //how do we i get the current state
  case  data::State::kIdle:
    fake_message = "CALIBRATE";
    break;
  case  data::State::kReady:
    fake_message = "LAUNCH";
    break;
  case  data::State::kRunComplete:
    fake_message = "SERVER_PROPULSION_GO";
    break;
  default:
    fake_message = "ACK";
    break;
  }
  log_.DBG("Fake-Telemetry", "Receiving fake message");
  return fake_message;
}

} //namespace telemetry
} //namespace hyped