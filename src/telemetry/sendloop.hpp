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
#pragma once

#include <rapidjson/writer.h>

#include <data/data.hpp>
#include <string>
#include <utils/concurrent/thread.hpp>

#include "main.hpp"

using rapidjson::StringBuffer;
using rapidjson::Writer;

namespace hyped {

using utils::Logger;
using utils::concurrent::Thread;

namespace telemetry {

class SendLoop : public Thread {
 public:
  explicit SendLoop(Logger &log, data::Data &data, Main *main_pointer);
  void run() override;

 private:
  std::string convertStateMachineState(data::State state);
  std::string convertModuleStatus(data::ModuleStatus module_status);
  Main &main_ref_;
  data::Data &data_;
};

}  // namespace telemetry
}  // namespace hyped
