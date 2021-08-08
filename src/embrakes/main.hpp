#pragma once

#include <data/data.hpp>
#include <embrakes/stepper.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

#include "fake_stepper.hpp"

namespace hyped {

using data::ModuleStatus;
using utils::Logger;
using utils::System;
using utils::concurrent::Thread;

namespace embrakes {
/*
 * @description This module handles the interaction with the embrakes.
 */
class Main : public Thread {
 public:
  /*
   * @brief Initialises essential variables
   */
  Main(uint8_t id, Logger &log);

  /*
   * @brief Cleans up previous allocations
   */
  ~Main();

  /*
   * @brief Checks for State kCalibrating to start retracting process
   */
  void run() override;

 private:
  Logger &log_;
  data::Data &data_;
  utils::System &sys_;
  data::StateMachine sm_data_;
  data::EmergencyBrakes em_brakes_;
  data::Telemetry tlm_data_;
  uint8_t command_pins_[2];    // GPIO pin numbers for sending commands to brakes
  uint8_t button_pins_[2];     // GPIO pin numbers for retrieving brake status
  StepperInterface *m_brake_;  // Stepper for electromagnetic brakes
  StepperInterface *f_brake_;  // Stepper for friction brakes
};

}  // namespace embrakes
}  // namespace hyped
