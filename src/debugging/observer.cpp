#include "observer.hpp"

#include <rapidjson/istreamwrapper.h>

#include <sensors/imu.hpp>
#include <sensors/main.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>
#include <utils/timer.hpp>

namespace hyped::debugging {

Observer::Observer(const std::string &path)
    : utils::concurrent::Thread(
      utils::Logger("OBSERVER", utils::System::getSystem().config_.log_level_debugger)),
      path_(path)
{
}

void Observer::run()
{
  auto &system = utils::System::getSystem();

  std::ofstream output_stream(path_);
  rapidjson::OStreamWrapper output_stream_wrapper(output_stream);
  JsonWriter json_writer(output_stream_wrapper);
  json_writer.StartArray();
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
    json_writer.EndObject();
    sleep(kSleepTimeMillis);
  }
  json_writer.EndArray();
  json_writer.Flush();
}

std::optional<std::unique_ptr<Observer>> Observer::fromFile(const std::string &path)
{
  const auto &system = utils::System::getSystem();
  utils::Logger log("OBSERVER", system.config_.log_level);
  std::ifstream input_stream(path);
  if (!input_stream.is_open()) {
    log.error("failed to open config file at %s", path.c_str());
    return std::nullopt;
  }
  rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
  rapidjson::Document document;
  document.ParseStream(input_stream_wrapper);
  if (document.HasParseError()) {
    log.error("failed to parse config file at %s", path.c_str());
    return std::nullopt;
  }
  if (!document.HasMember("observer")) {
    log.error("missing required field 'observer' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  const auto config_object = document["observer"].GetObject();
  if (!config_object.HasMember("output_path")) {
    log.error("missing required field 'observer.output_path' in configuration file at %s",
              path.c_str());
    return std::nullopt;
  }
  const auto output_path = config_object["output_path"].GetString();
  auto observer          = std::make_unique<Observer>(output_path);
  const auto imu_pins    = sensors::Main::imuPinsFromFile(log, path);
  if (imu_pins) {
    log.debug("adding imu tasks");
    for (const auto imu_pin : *imu_pins) {
      observer->addImuTask(imu_pin);
    }
  }
  if (system.config_.use_fake_trajectory) {
    const auto fake_trajectory_optional = sensors::FakeTrajectory::fromFile(path);
    if (!fake_trajectory_optional) {
      log.error("failed to initialise fake trajectory");
      return std::nullopt;
    }
    log.debug("adding fake imu tasks");
    const auto fake_trajectory
      = std::make_shared<sensors::FakeTrajectory>(*fake_trajectory_optional);
    const auto fake_imus = sensors::FakeImu::fromFile(path, fake_trajectory);
    if (fake_imus) { observer->addFakeImuTasks(*fake_imus); }
  }
  return observer;
}

void Observer::addImuTask(const uint8_t pin)
{
  auto imu = std::make_shared<sensors::Imu>(pin, false);
  char name_buffer[16];
  snprintf(name_buffer, 16, "imu-%u", pin);
  Task imu_task;
  imu_task.name    = name_buffer;
  imu_task.handler = [imu](JsonWriter &json_writer) {
    const auto imu_data = imu->getData();
    json_writer.Key("operational");
    json_writer.Bool(imu_data.operational);
    json_writer.Key("acceleration");
    json_writer.StartArray();
    json_writer.Double(imu_data.acc[0]);
    json_writer.Double(imu_data.acc[1]);
    json_writer.Double(imu_data.acc[2]);
    json_writer.EndArray();
  };
  tasks_.push_back(imu_task);
}

void Observer::addFakeImuTasks(std::vector<sensors::FakeImu> fake_imus)
{
  uint32_t i = 0;
  char name_buffer[16];
  for (auto fake_imu : fake_imus) {
    snprintf(name_buffer, 16, "fake_imu-%u", i++);
    auto fake_imu_ptr = std::make_shared<sensors::FakeImu>(fake_imu);
    Task fake_imu_task;
    fake_imu_task.name    = name_buffer;
    fake_imu_task.handler = [fake_imu_ptr](JsonWriter &json_writer) {
      const auto fake_imu_data = fake_imu_ptr->getData();
      json_writer.Key("operational");
      json_writer.Bool(fake_imu_data.operational);
      json_writer.Key("acceleration");
      json_writer.StartArray();
      json_writer.Double(fake_imu_data.acc[0]);
      json_writer.Double(fake_imu_data.acc[1]);
      json_writer.Double(fake_imu_data.acc[2]);
      json_writer.EndArray();
    };
    tasks_.push_back(fake_imu_task);
  }
}

}  // namespace hyped::debugging
