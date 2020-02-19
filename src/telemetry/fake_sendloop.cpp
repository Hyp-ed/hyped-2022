/*
 * Author: J. Ridley
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
#include "fake_sendloop.hpp"
#include "writer.hpp"

namespace hyped {
namespace telemetry {

FakeSendLoop::FakeSendLoop(Logger& log, data::Data& data, Main* main_pointer)
  : Thread {log},
    main_ref_ {*main_pointer},
    data_ {data}
{
  log_.DBG("Telemetry", "Fake Telemetry SendLoop thread object created");
}

void FakeSendLoop::run()
{
  log_.DBG("Telemetry", "Fake Telemetry SendLoop thread started");

  while (true) {
    Writer writer(data_);

    writer.start();
    writer.packCrucialData();
    writer.packStatusData();
    writer.packAdditionalData();
    writer.end();

    Thread::sleep(100);
  }

  log_.DBG("Telemetry", "Exiting Fake Telemetry SendLoop thread");
}

}  // namespace telemetry
}  // namespace hyped
