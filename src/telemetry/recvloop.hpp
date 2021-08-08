#pragma once

#include "data/data.hpp"
#include "main.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped {

namespace telemetry {

class RecvLoop : public Thread {
 public:
  explicit RecvLoop(Logger &log, data::Data &data, Main *main_pointer);
  void run() override;

 private:
  Main &main_ref_;
  data::Data &data_;
};

}  // namespace telemetry
}  // namespace hyped
