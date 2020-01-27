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

#include <csignal>
#include "telemetry/signalhandler.hpp"

namespace hyped {
namespace telemetry {

bool SignalHandler::receivedSigPipeSignal = false;

// didn't really want to set this as a static variable and hardcode logger values
// but don't really see any other way (sys.verbose_tlm gets set at runtime)
Logger SignalHandler::log_(true, 2);

SignalHandler::SignalHandler()
{
  std::signal(SIGPIPE, SignalHandler::sigPipeHandler);  // maybe throw excptn if std::signal fails??
  log_.DBG("Telemetry", "Initialized SignalHandler and set SIGPIPE handler");
}

bool SignalHandler::gotSigPipeSignal()
{
  return receivedSigPipeSignal;
}

void SignalHandler::sigPipeHandler(int signum)
{
  receivedSigPipeSignal = true;
  log_.ERR("Telemetry", "Received SIGPIPE");
}

}  // namespace telemetry
}  // namespace hyped
