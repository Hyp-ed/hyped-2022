/*
 * Author: Neil Weidinger
 * Organisation: HYPED
 * Date: April 2019
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

#include "telemetry/recvloop.hpp"
#include "telemetry/telemetrydata/message.pb.h"

namespace hyped {

using data::ModuleStatus;

namespace telemetry {

RecvLoop::RecvLoop(Logger &log, data::Data& data, Main* main_pointer)
  : Thread {log},
    main_ref_ {*main_pointer},
    data_ {data}
{
  log_.DBG("Telemetry", "Telemetry RecvLoop thread object created");
}

void RecvLoop::run()
{
  log_.DBG("Telemetry", "Telemetry RecvLoop thread started");
  telemetry_data::ServerToClient msg;
  // not sure whether to put this in or ouside of loop
  data::Telemetry telem_data_struct = data_.getTelemetryData();

  while (true) {
    try {
      msg = main_ref_.client_.receiveData();
    }
    catch (std::exception& e) {
      log_.ERR("Telemetry", "%s", e.what());

      telem_data_struct.module_status = ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telem_data_struct);

      break;
    }

    switch (msg.command()) {
      case telemetry_data::ServerToClient::ACK:
        log_.INFO("Telemetry", "FROM SERVER: ACK");
        break;
      case telemetry_data::ServerToClient::STOP:
        log_.INFO("Telemetry", "FROM SERVER: STOP");
        telem_data_struct.emergency_stop_command = true;
        break;
      case telemetry_data::ServerToClient::CALIBRATE:
        log_.INFO("Telemetry", "FROM SERVER: CALIBRATE");
        telem_data_struct.calibrate_command = true;
        break;
      case telemetry_data::ServerToClient::LAUNCH:
        log_.INFO("Telemetry", "FROM SERVER: LAUNCH");
        telem_data_struct.launch_command = true;
        break;
      case telemetry_data::ServerToClient::RESET:
        log_.INFO("Telemetry", "FROM SERVER: RESET");
        telem_data_struct.reset_command = true;
        break;
      case telemetry_data::ServerToClient::SERVICE_PROPULSION_GO:
        log_.INFO("Telemetry", "FROM SERVER: SERVICE_PROPULSION_GO");
        telem_data_struct.service_propulsion_go = true;
        break;
      case telemetry_data::ServerToClient::SERVICE_PROPULSION_STOP:
        log_.INFO("Telemetry", "FROM SERVER: SERVICE_PROPULSION_STOP");
        telem_data_struct.service_propulsion_go = false;
        break;
      case telemetry_data::ServerToClient::NOMINAL_BRAKING:
        log_.INFO("Telemetry", "FROM SERVER: NOMINAL_BRAKING");
        telem_data_struct.nominal_braking_command = true;
        break;
      case telemetry_data::ServerToClient::NOMINAL_RETRACT:
        log_.INFO("Telemetry", "FROM SERVER: NOMINAL_RETRACT");
        telem_data_struct.nominal_braking_command = false;
        break;
      default:
        log_.ERR("Telemetry", "Unrecognized input from server, ENTERING CRITICAL FAILURE");
        telem_data_struct.module_status = ModuleStatus::kCriticalFailure;
        break;
    }

    data_.setTelemetryData(telem_data_struct);
  }

  log_.DBG("Telemetry", "Exiting Telemetry RecvLoop thread");
}

}  // namespace telemetry
}  // namespace hyped
