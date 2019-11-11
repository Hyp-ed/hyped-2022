/*
* Author: Kornelija Sukyte
* Organisation: HYPED
* Date:
* Description: Entrypoint class to the embrake module, started in it's own thread.
*
*    Copyright 2019 HYPED
*    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
*    except in compliance with the License. You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
*    Unless required by applicable law or agreed to in writing, software distributed under
*    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
*    either express or implied. See the License for the specific language governing permissions and
*    limitations under the License.
*/

#ifndef EMBRAKES_MAIN_HPP_
#define EMBRAKES_MAIN_HPP_

#include "utils/concurrent/thread.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "data/data.hpp"

#include "embrakes/stepper.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::Logger;
using utils::System;
using data::ModuleStatus;

namespace embrakes {
/*
 * @description This module handles the interaction with the embrakes.
*/
class Main : public Thread
{
  public:
    /*
    * @brief Initialises essential variables
    */
    Main(uint8_t id, Logger &log);

    /*
    * @brief Checks for State kCalibrating to start retracting process
    */
    void run() override;

  private:
    Logger&                log_;
    data::Data&            data_;
    utils::System&         sys_;
    data::StateMachine     sm_data_;
    data::EmergencyBrakes  em_brakes_;
    data::Telemetry        tlm_data_;
    int                    command_pins_[4];
    int                    button_pins_[4];
    Stepper*               brake_1;
    // Stepper*               brake_2;
    // Stepper*               brake_3;
    // Stepper*               brake_4;
};

}}

#endif  // EMBRAKES_MAIN_HPP_
