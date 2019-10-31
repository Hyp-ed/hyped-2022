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

#include "telemetry/main.hpp"
#include "telemetry/telemetrydata/message.pb.h"
#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::Logger;
using batteriesMsg = telemetry_data::ClientToServer::Batteries;

namespace telemetry {

class SendLoop: public Thread {
  public:
    explicit SendLoop(Logger &log, data::Data& data, Main* main_pointer);
    void run() override;

  private:
    void packNavigationMessage(telemetry_data::ClientToServer& msg);
    void packStateMachineMessage(telemetry_data::ClientToServer& msg);
    void packMotorsMessage(telemetry_data::ClientToServer& msg);
    void packBatteriesMessage(telemetry_data::ClientToServer& msg);
    template<std::size_t SIZE>
    void packLpBatteryDataMessage(batteriesMsg& batteries_msg, std::array<data::BatteryData, SIZE>& battery_data_array); // NOLINT
    template<std::size_t SIZE>
    void packHpBatteryDataMessage(batteriesMsg& batteries_msg, std::array<data::BatteryData, SIZE>& battery_data_array); // NOLINT
    void packBatteryDataMessageHelper(batteriesMsg::BatteryData& battery_data_msg, data::BatteryData& battery_data); // NOLINT
    void packSensorsMessage(telemetry_data::ClientToServer& msg);
    void packTemperatureMessage(telemetry_data::ClientToServer& msg);
    void packEmergencyBrakesMessage(telemetry_data::ClientToServer& msg);
    Main&                   main_ref_;
    data::Data&             data_;
};

}  // namespace telemetry
}  // namespace hyped

#endif  // TELEMETRY_SENDLOOP_HPP_
