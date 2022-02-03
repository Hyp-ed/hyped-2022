#pragma once

#include "navigation.hpp"

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped {

namespace navigation {

class Main : public utils::concurrent::Thread {
 public:
  explicit Main(uint8_t id, utils::Logger &log);
  void run() override;
  bool isCalibrated();

 private:
  utils::Logger &log_;
  utils::System &sys_;
  navigation::Navigation nav_;
};

}  // namespace navigation
}  // namespace hyped
