#pragma once

#include <fstream>
#include <functional>
#include <vector>

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include <utils/concurrent/thread.hpp>

namespace hyped::debugging {

class Observer : utils::concurrent::Thread {
 public:
  static constexpr uint32_t kSleepTimeMillis = 10;
  struct Task {
    std::string name;
    std::function<void(rapidjson::Writer<rapidjson::StringBuffer> &)> handler;
  };
  Observer(const std::string &path, const std::vector<Task> &tasks);

  void run() override;

  static std::optional<Observer> fromFile(const std::string &path);

 private:
  const std::vector<Task> tasks_;
  const std::string path_;
};

}  // namespace hyped::debugging
