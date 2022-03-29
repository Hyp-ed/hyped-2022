#pragma once

#include "stepper.hpp"

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::brakes {

class FakeStepper : public IStepper {
 public:
  /**
   * @brief Construct a new Stepper object
   * @param log, node id
   */
  FakeStepper(utils::Logger &log, uint8_t id);

  /**
   * @brief Deconstruct a Stepper object even if behind `IStepper *`
   */
  ~FakeStepper() {}

  /**
   * @brief {checks if brake's button is pressed, notes change in the data
   * struct}
   */
  void checkHome() override;

  /**
   * @brief sends retract message
   */
  void sendRetract() override;

  /**
   * @brief sends clamp message
   */
  void sendClamp() override;

  /**
   * @brief checks for brake failure during acceleration
   */
  void checkAccFailure() override;

  void checkBrakingFailure() override;

  bool checkClamped() override;

 private:
  utils::Logger &log_;
  data::Data &data_;
  data::Brakes brakes_data_;
  uint8_t brake_id_;
  uint8_t is_clamped_;
  bool fake_button_;
};

}  // namespace hyped::brakes
