#pragma once

#include "state_processor.hpp"

#include <data/data.hpp>
#include <utils/concurrent/barrier.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>
#include <utils/timer.hpp>

namespace hyped {

namespace motor_control {

constexpr int32_t kNumMotors = 4;

class Main : public utils::concurrent::Thread {
 public:
  Main(const uint8_t id, utils::Logger &log);

  /**
   * @brief {This function is the entrypoint to the propulsion module and reacts to the certain
   * states}
   */
  void run() override;

 private:
  bool is_running_;
  utils::Logger &log_;
  StateProcessor *state_processor_;
  data::State current_state_;
  data::State previous_state_;
  /**
   * @brief   Returns true if the pod state has changed since the last check.
   */
  bool handleTransition();

  void handleCriticalFailure(data::Data &data, data::Motors &motor_data);
};

}  // namespace motor_control
}  // namespace hyped
