/*
* Author: Gregor Konzett
* Organisation: HYPED
* Date: 1.4.2019
* Description: Handles the different states of the state machine
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

#ifndef PROPULSION_STATE_PROCESSOR_HPP_
#define PROPULSION_STATE_PROCESSOR_HPP_

#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/timer.hpp"

#include "propulsion/state_processor_interface.hpp"
#include "propulsion/controller_interface.hpp"
#include "propulsion/controller.hpp"
#include "propulsion/fake_controller.hpp"
#include "propulsion/RPM_regulator.hpp"
#include "data/data.hpp"

namespace hyped
{

namespace motor_control
{

using utils::Logger;
using utils::System;
using data::Batteries;
using utils::Timer;
using data::Navigation;
using data::Data;
using data::Motors;

class StateProcessor : public StateProcessorInterface
{
  public:
    /**
     * @brief {Initializes the state processors with the amount of motors and the logger}
     * */
    StateProcessor(int motorAmount, Logger &log);

    /**
     * @brief { Sends the desired settings to the motors }
     */
    void initMotors() override;

    /**
     * @brief { Changes the state of the motor controller to preOperational }
     */
    void enterPreOperational() override;

    /**
     * @brief { Stops all motors }
     */
    void quickStopAll() override;

    /**
     * @brief { Checks the motor controller's health }
     */
    void healthCheck() override;

    /**
     * @brief { Checks if the motor controller's error registers }
     */
    bool getFailure() override;

    /**
     * @brief { Tells the controllers to start accelerating the motors }
     */
    void accelerate() override;

    /**
     * @brief { Returns if the motors are initialised already }
     */
    bool isInitialized() override;

    /**
    * @brief Exits the tube with low velocity
    * */
    void servicePropulsion() override;

    /**
     * @brief Returns if a critical error ocurred
     * */
    bool isCriticalFailure() override;

    /**
     * @brief sends the enter operational command
     */
    void sendOperationalCommand();

  protected:
    /**
     * @brief { Registers the controllers to handle CAN transmissions }
     */
    void registerControllers() override;

    /**
     * @brief { Configures the controllers }
     */
    void configureControllers() override;

    /**
     * @brief { Send settings data to the motors }
     */
    void prepareMotors() override;

    /**
     * @brief Calculate the Average rpm of all motors
     *
     * @param controllers
     * @return int32_t
     */
    int32_t calcAverageRPM(ControllerInterface** controllers);

    /**
     * @brief calculate the max Current drawn out of all the motors
     *
     * @param controllers
     * @return int32_t
     */
    int16_t calcMaxCurrent();

    /**
     * @brief Calculate the max temperature out of all the motors
     *
     * @param controllers
     * @return int32_t
     */
    int32_t calcMaxTemp(ControllerInterface** controllers);

    bool useFakeController;
    Logger &log_;
    System &sys_;
    Data   &data_;
    Motors motor_data_;
    int motorAmount;
    bool initialized;
    bool criticalError;
    int32_t servicePropulsionSpeed;
    float speed;
    ControllerInterface **controllers;
    RPM_Regulator regulator;
    float velocity;
    Navigation navigationData;
    uint64_t accelerationTimestamp;
    Timer accelerationTimer;
};

}}  // hyped::motor_control

#endif  // PROPULSION_STATE_PROCESSOR_HPP_
