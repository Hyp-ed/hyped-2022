#pragma once

#include "state.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped {

using data::ModuleStatus;
using utils::Logger;
using utils::concurrent::Thread;

namespace state_machine {

class State;  // Forward declaration

class Main : public Thread {
 public:
  explicit Main(uint8_t id, Logger &log);

  /**
   *  @brief  Runs state machine thread.
   */
  void run() override;

  /*
   * @brief  Current state of the pod
   */
  State *current_state_;
};

}  // namespace state_machine
}  // namespace hyped
