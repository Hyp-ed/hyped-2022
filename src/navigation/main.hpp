#pragma once

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

#include "navigation.hpp"

namespace hyped {

using data::Data;
using data::State;
using hyped::data::ModuleStatus;
using hyped::data::StateMachine;
using utils::Logger;
using utils::System;
using utils::concurrent::Thread;

namespace navigation {

class Main : public Thread {
 public:
  explicit Main(uint8_t id, Logger &log);
  void run() override;
  bool isCalibrated();

 private:
  Logger &log_;
  System &sys_;
  Navigation nav_;
};

}  // namespace navigation
}  // namespace hyped
