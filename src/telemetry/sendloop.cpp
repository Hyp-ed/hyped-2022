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

#include <string>
#include "sendloop.hpp"
#include "writer.hpp"

namespace hyped {
namespace telemetry {

SendLoop::SendLoop(Logger& log, data::Data& data, Main* main_pointer)
  : Thread {log},
    main_ref_ {*main_pointer},
    data_ {data}
{
  log_.DBG("Telemetry", "Telemetry SendLoop thread object created");
}

void SendLoop::run()
{
  log_.DBG("Telemetry", "Telemetry SendLoop thread started");

  while (true) {
    Writer writer(data_);

    writer.start();
    writer.packCrucialData();
    writer.packStatusData();
    writer.packAdditionalData();
    writer.end();

    if (!main_ref_.client_->sendData(writer.getString())) {
      log_.ERR("Telemetry", "Error sending message");
      data::Telemetry telem_data_struct = data_.getTelemetryData();
      telem_data_struct.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telem_data_struct);

      break;
    }

    Thread::sleep(100);
  }

  log_.DBG("Telemetry", "Exiting Telemetry SendLoop thread");
}

}  // namespace telemetry
}  // namespace hyped
