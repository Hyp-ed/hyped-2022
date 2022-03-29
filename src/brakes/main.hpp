#pragma once

#include "fake_brake.hpp"

#include <brakes/brake.hpp>
#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::brakes {

struct BrakePins {
  uint8_t command_pin;
  uint8_t button_pin;
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
   * @brief Checks for State kCalibrating to start retracting process
   */
  void run() override;

  void checkEngaged();
  void checkRetracted();
  void engage();
  void retract();

  static std::optional<std::vector<BrakePins>> pinsFromFile(utils::Logger &log,
                                                            const std::string &path);

 private:
  utils::System &sys_;
  data::Data &data_;
  data::Brakes brakes_data_;
  std::unique_ptr<IBrake> magnetic_brake_;  // Brake for electromagnetic brakes
  std::unique_ptr<IBrake> friction_brake_;  // Brake for friction brakes
};

}  // namespace hyped::brakes
