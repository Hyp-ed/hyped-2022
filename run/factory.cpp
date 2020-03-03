/*
 * Authors : Martin Kristien
 * Organisation: HYPED
 * Date: 3. Dec 2019
 * Description:
 * Demo executable showing how to use the driver interface abstraction.
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
 *
 */

#include "utils/system.hpp"
#include "utils/config.hpp"
#include "sensors/interface.hpp"
#include "demo/interface.hpp"
using namespace hyped;

int main(int argc, char* argv[])
{
  utils::System::parseArgs(argc, argv);
  utils::System& sys = utils::System::getSystem();

  sensors::ImuInterface* imu = sys.config->interfaceFactory.getImuInterfaceInstance();

  demo::DemoInterface* demo = sys.config->interfaceFactory.getDemoInterfaceInstance();
  demo->printYourName();

  return 0;
}
