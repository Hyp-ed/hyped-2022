#pragma once

#include "brake.hpp"

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::brakes {

class FakeBrake : public IBrake {
 public:
  /**
   * @brief Construct a new Brake object
   * @param log, node id
   */
  FakeBrake(const uint8_t id);

  /**
   * @brief Deconstruct a Brake object even if behind `IBrake *`
   */
  ~FakeBrake() {}

  /**
   * @brief sends retract message
   */
  void retract() override;

  /**
   * @brief sends clamp message
   */
  void engage() override;

  bool isEngaged() override;

 private:
  utils::Logger log_;
  data::Data &data_;
  data::Brakes brakes_data_;
  const uint8_t id_;
  bool fake_button_;
};

}  // namespace hyped::brakes
