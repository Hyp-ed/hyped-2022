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

#ifndef TELEMETRY_MAIN_HPP_
#define TELEMETRY_MAIN_HPP_

#include "telemetry/client.hpp"
#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::Logger;

namespace telemetry {

class Main: public Thread {
  public:
    Main(uint8_t id, Logger& log);
    void run() override;

  private:
    friend class SendLoop;
    friend class RecvLoop;
    data::Data& data_;
    Client client_;
};

}  // namespace telemetry
}  // namespace hyped

#endif  // TELEMETRY_MAIN_HPP_
