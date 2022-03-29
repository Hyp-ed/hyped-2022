#pragma once

#include "fake_stepper.hpp"

#include <brakes/stepper.hpp>
#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::brakes {

struct Pins {
  std::array<std::uint8_t, data::Brakes::kNumBrakes> command_pins;
  std::array<std::uint8_t, data::Brakes::kNumBrakes> button_pins;
};

/*
 * @description This module handles the interaction with the brakes.
 */
class Main : public utils::concurrent::Thread {
 public:
  /*
   * @brief Initialises essential variables
   */
  Main();

  /*
   * @brief Cleans up previous allocations
   */
  ~Main();

  /*
   * @brief Checks for State kCalibrating to start retracting process
   */
  void run() override;

 private:
  std::optional<Pins> pinsFromFile(const std::string &path);

  utils::System &sys_;
  data::Data &data_;
  data::StateMachine sm_data_;
  data::Brakes brakes_;
  data::Telemetry tlm_data_;
  IStepper *m_brake_;  // Stepper for electromagnetic brakes
  IStepper *f_brake_;  // Stepper for friction brakes
};

}  // namespace hyped::brakes
