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

#ifndef TELEMETRY_FAKE_RECVLOOP_HPP_
#define TELEMETRY_FAKE_RECVLOOP_HPP_

#include "telemetry/main.hpp"
#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped {

namespace telemetry {

class FakeRecvLoop: public Thread {
  public:
    explicit FakeRecvLoop(Logger &log, data::Data& data, Main* main_pointer);
    std::string receiveFakeData();
    void run() override;

  private:
    Main& main_ref_;
    data::Data& data_;
};

}  // namespace telemetry
}  // namespace hyped

#endif  // FAKE_TELEMETRY_RECVLOOP_HPP_
