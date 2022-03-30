#pragma once

#include <fstream>
#include <functional>
#include <vector>

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include <sensors/bms_manager.hpp>
#include <sensors/fake_imu.hpp>
#include <sensors/imu_manager.hpp>
#include <sensors/main.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::debugging {

class Observer : public utils::concurrent::Thread {
 public:
  static constexpr uint64_t kSleepTimeMillis = 10;
  using JsonWriter                           = rapidjson::Writer<rapidjson::OStreamWrapper>;
  struct Task {
    std::string name;
    std::function<void(JsonWriter &)> handler;
  };

  Observer(const std::string &path);
  void run() override;

  static std::optional<std::unique_ptr<Observer>> fromFile(const std::string &path);

 private:
  std::vector<Task> tasks_;
  const std::string path_;

  void addImuTask(const uint8_t pin);
  void addFakeImuTasks(std::vector<std::unique_ptr<sensors::FakeImu>> fake_imus);
  void addImuManagerTask(const sensors::ImuPins &imu_pins);
  void addFakeImuManagerTask(std::shared_ptr<sensors::FakeTrajectory> fake_trajectory);
};

}  // namespace hyped::debugging
