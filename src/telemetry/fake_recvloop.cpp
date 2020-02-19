/*
 * Author: J. Ridley
 * Organisation: HYPED
 * Date: April 2019
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

#include <string>
#include "telemetry/fake_recvloop.hpp"

namespace hyped {

using data::ModuleStatus;

namespace telemetry {

FakeRecvLoop::FakeRecvLoop(Logger &log, data::Data& data, Main* main_pointer)
  : Thread {log},
    main_ref_ {*main_pointer},
    data_ {data}
{
  log_.DBG("Fake-Telemetry", "Fake Telemetry RecvLoop thread object created");
}

/*
 * Fake data will automatically "receive" the message to transition
 * to the next state as soon as statemachine enters the prerequisite state.
 * Sends a new message every second to avoid clogging.
 */
std::string FakeRecvLoop::receiveFakeData()
{
  Thread::sleep(1000);
  std::string fake_message;
  switch (data_.getStateMachineData().current_state) {
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

void FakeRecvLoop::run()
{
  log_.DBG("Fake-Telemetry", "Fake Telemetry RecvLoop thread started");

  data::Telemetry telem_data_struct = data_.getTelemetryData();
  std::string message;

  while (true) {
    try {
      message = receiveFakeData();
    }
    catch (std::exception& e) {
      log_.ERR("Fake-Telemetry", "%s", e.what());

      telem_data_struct.module_status = ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telem_data_struct);

      break;
    }

    if (message == "ACK") {
      log_.INFO("Fake-Telemetry", "FROM SERVER: ACK");
    } else if (message == "STOP") {
      log_.INFO("Fake-Telemetry", "FROM SERVER: STOP");
      telem_data_struct.emergency_stop_command = true;
    } else if (message == "CALIBRATE") {
      log_.INFO("Fake-Telemetry", "FROM SERVER: CALIBRATE");
      telem_data_struct.calibrate_command = true;
    } else if (message == "LAUNCH") {
      log_.INFO("Fake-Telemetry", "FROM SERVER: LAUNCH");
      telem_data_struct.launch_command = true;
    } else if (message == "RESET") {
      log_.INFO("Fake-Telemetry", "FROM SERVER: RESET");
      telem_data_struct.reset_command = true;
    } else if (message == "SERVER_PROPULSION_GO") {
      log_.INFO("Fake-Telemetry", "FROM SERVER: SERVICE_PROPULSION_GO");
      telem_data_struct.service_propulsion_go = true;
    } else if (message == "SERVER_PROPULSION_STOP") {
      log_.INFO("Fake-Telemetry", "FROM SERVER: SERVICE_PROPULSION_STOP");
      telem_data_struct.service_propulsion_go = false;
    } else if (message == "NOMINAL_BRAKING") {
      log_.INFO("Fake-Telemetry", "FROM SERVER: NOMINAL_BRAKING");
      telem_data_struct.nominal_braking_command = true;
    } else if (message == "NOMINAL_RETRACT") {
      log_.INFO("Fake-Telemetry", "FROM SERVER: NOMINAL_RETRACT");
      telem_data_struct.nominal_braking_command = false;
    } else {
      log_.ERR("Fake-Telemetry", "Unrecognized input from server, ENTERING CRITICAL FAILURE");
      telem_data_struct.module_status = ModuleStatus::kCriticalFailure;
    }

    data_.setTelemetryData(telem_data_struct);

    // Once SVP GO is entered wait 5 seconds before changing state again
    // for the sake of testing SVP code
    if (message == "SERVER_PROPULSION_GO") {
      Thread::sleep(5000);
      telem_data_struct.service_propulsion_go = false;
      data_.setTelemetryData(telem_data_struct);
    }
  }

  log_.DBG("Fake-Telemetry", "Exiting Fake Telemetry RecvLoop thread");
}

}  // namespace telemetry
}  // namespace hyped
