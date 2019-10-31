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

#ifndef TELEMETRY_UTILS_HPP_
#define TELEMETRY_UTILS_HPP_

#include "telemetry/telemetrydata/message.pb.h"
#include "data/data.hpp"

namespace hyped {

using ProtoModuleStatus = telemetry_data::ClientToServer::ModuleStatus;
using ProtoState        = telemetry_data::ClientToServer::StateMachine::State;
using DataModuleStatus  = data::ModuleStatus;
using DataState         = data::State;

namespace telemetry {

class Utils {
  public:
    static ProtoModuleStatus moduleStatusEnumConversion(DataModuleStatus status);
    static ProtoState stateEnumConversion(DataState state);
};

}  // namespace telemetry
}  // namespace hyped

#endif  // TELEMETRY_UTILS_HPP_
