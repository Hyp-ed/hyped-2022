#pragma once

#include "state.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <state_machine/state.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped::state_machine {

class Main : public utils::concurrent::Thread {
 public:
  Main();

  /**
   *  @brief  Runs state machine thread.
   */
  void run() override;

  /*
   * @brief  Current state of the pod
   */
  State *current_state_;
};

}  // namespace hyped::state_machine
