/*
 * Author: Kornelija Sukyte
 * Organisation: HYPED
 * Date: 29/6/19
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

#include "utils/concurrent/thread.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"
#include "utils/system.hpp"
#include "embrakes/stepper.hpp"

using hyped::utils::concurrent::Thread;
using hyped::utils::Logger;
using hyped::utils::Timer;
using hyped::embrakes::Stepper;
using hyped::utils::System;

int main(int argc, char* argv[]) {
    System::parseArgs(argc, argv);
    Logger log(true, 3);
    
    // Stepper* stepper;
    // stepper = new Stepper(log, 20);
    // stepper->registerStepper();
    // stepper->sendRetract();
    // log.INFO("Brakes_test", "Retract message sent");
    // Thread::sleep(10000);
    // stepper->sendClamp();
    // log.INFO("Brakes_test", "Clamp message sent");

}