#include "observer.hpp"

#include <rapidjson/ostreamwrapper.h>

#include <utils/logger.hpp>
#include <utils/system.hpp>
#include <utils/timer.hpp>

namespace hyped::debugging {

Observer::Observer(const std::string &path, const std::vector<Task> &tasks)
    : utils::concurrent::Thread(
      utils::Logger("OBSERVER", utils::System::getSystem().config_.log_level_debugger)),
      tasks_(tasks),
      path_(path)
{
}

void Observer::run()
{
  auto &system = utils::System::getSystem();

  std::ofstream output_stream(path_);
  rapidjson::StringBuffer string_buffer;
  rapidjson::Writer<rapidjson::StringBuffer> json_writer(string_buffer);

  while (is_running_ && system.isRunning()) {
    // Write CSV line
    json_writer.StartObject();
    json_writer.Key("time_micros");
    json_writer.Uint64(utils::Timer::getTimeMicros());
    for (auto &task : tasks_) {
      json_writer.Key(task.name.c_str());
      json_writer.StartObject();
      task.handler(json_writer);
      json_writer.EndObject();
    }
    output_stream << std::endl;
    json_writer.EndObject();
    output_stream << string_buffer.GetString();
    output_stream << ';' << std::endl;
    output_stream.flush();
    string_buffer.Clear();
    json_writer.Reset(string_buffer);
    sleep(kSleepTimeMillis);
  }
}

static std::optional<Observer> fromFile(const std::string &path)
{
  return std::nullopt;
}

}  // namespace hyped::debugging
