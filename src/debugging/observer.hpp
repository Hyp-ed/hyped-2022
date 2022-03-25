#pragma once

#include <fstream>
#include <functional>
#include <vector>

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include <sensors/bms_manager.hpp>
#include <sensors/fake_imu.hpp>
#include <sensors/imu_manager.hpp>
#include <sensors/interface.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::debugging {

class Observer : public utils::concurrent::Thread {
 public:
  static constexpr uint32_t kSleepTimeMillis = 10;
  struct Task {
    std::string name;
    std::function<void(rapidjson::Writer<rapidjson::StringBuffer> &)> handler;
  };

  Observer(const std::string &path);
  void run() override;

  static std::optional<std::unique_ptr<Observer>> fromFile(const std::string &path);

 private:
  std::vector<Task> tasks_;
  const std::string path_;

  void addImuTask(const uint8_t pin);
  void addFakeImuTasks(std::vector<sensors::FakeImu> fake_imus);
};

}  // namespace hyped::debugging
