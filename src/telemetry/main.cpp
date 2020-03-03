/*
 * Author: Neil Weidinger
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

#include "utils/system.hpp"
#include "telemetry/main.hpp"
#include "telemetry/sendloop.hpp"
#include "telemetry/recvloop.hpp"
#include "telemetry/client_interface.hpp"

namespace hyped {

using data::ModuleStatus;

namespace telemetry {

Main::Main(uint8_t id, Logger& log)
  : Thread {id, log},
    data_ {data::Data::getInstance()},
    client_ {
      utils::System::getSystem().config->interfaceFactory.getClientInterfaceInstance()
    }
{
  log_.DBG("Telemetry", "Telemetry Main thread object created");
}

void Main::run()
{
  // check if telemetry is disabled
  hyped::utils::System& sys = hyped::utils::System::getSystem();
  if (sys.telemetry_off) {
    log_.DBG("Telemetry", "Telemetry is disabled");
    log_.DBG("Telemetry", "Exiting Telemetry Main thread");
    return;
  }

  log_.DBG("Telemetry", "Telemetry Main thread started");

  data::Telemetry telem_data_struct = data_.getTelemetryData();


  try {
    client_->connect();
  }
  catch (std::exception& e) {
    log_.ERR("Telemetry", e.what());
    log_.ERR("Telemetry", "Exiting Telemetry Main thread (due to error connecting)");

    telem_data_struct.module_status = ModuleStatus::kCriticalFailure;
    data_.setTelemetryData(telem_data_struct);

    return;
  }


  telem_data_struct.module_status = ModuleStatus::kInit;
  data_.setTelemetryData(telem_data_struct);

  SendLoop sendloop_thread {log_, data_, this};
  RecvLoop recvloop_thread {log_, data_, this};
  sendloop_thread.start();
  recvloop_thread.start();
  sendloop_thread.join();
  recvloop_thread.join();


  log_.DBG("Telemetry", "Exiting Telemetry Main thread");
}

}  // namespace telemetry
}  // namespace hyped
