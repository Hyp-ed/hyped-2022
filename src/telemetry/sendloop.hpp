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

#ifndef TELEMETRY_SENDLOOP_HPP_
#define TELEMETRY_SENDLOOP_HPP_

#include <rapidjson/writer.h>
#include <string>
#include "telemetry/main.hpp"
#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

using rapidjson::Writer;
using rapidjson::StringBuffer;

namespace hyped {

using utils::concurrent::Thread;
using utils::Logger;

namespace telemetry {

class SendLoop: public Thread {
  public:
    explicit SendLoop(Logger &log, data::Data& data, Main* main_pointer);
    void run() override;

  private:
    void packNavigationMessage(Writer<StringBuffer>& writer);
    void packStateMachineMessage(Writer<StringBuffer>& writer);
    void packMotorsMessage(Writer<StringBuffer>& writer);
    void packBatteriesMessage(Writer<StringBuffer>& writer);
    template<std::size_t SIZE>
    void packLpBatteryDataMessage(Writer<StringBuffer>& writer, std::array<data::BatteryData, SIZE>& battery_data_array); // NOLINT
    template<std::size_t SIZE>
    void packHpBatteryDataMessage(Writer<StringBuffer>& writer, std::array<data::BatteryData, SIZE>& battery_data_array); // NOLINT
    void packBatteryDataMessageHelper(bool HP, Writer<StringBuffer>& writer, data::BatteryData& battery_data); // NOLINT
    void packSensorsMessage(Writer<StringBuffer>& writer);
    void packTemperatureMessage(Writer<StringBuffer>& writer);
    void packEmergencyBrakesMessage(Writer<StringBuffer>& writer);
    std::string convertStateMachineState(data::State state);
    std::string convertModuleStatus(data::ModuleStatus module_status);
    Main&                   main_ref_;
    data::Data&             data_;
};

}  // namespace telemetry
}  // namespace hyped

#endif  // TELEMETRY_SENDLOOP_HPP_
