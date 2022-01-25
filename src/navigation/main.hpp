#pragma once

#include "navigation.hpp"

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::navigation {

class Main : public utils::concurrent::Thread {
 public:
  Main();
  void run() override;
  bool isCalibrated();

 private:
  utils::System &sys_;
  Navigation nav_;
};

}  // namespace hyped::navigation
