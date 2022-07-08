#include "observer.hpp"

#include <sstream>

#include <rapidjson/istreamwrapper.h>

#include <sensors/fake_temperature.hpp>
#include <sensors/fake_brake_pressure.hpp>
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
  if (!document.HasMember("debugger")) {
    log.error("missing required field 'debugger' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  const auto config_object = document["debugger"].GetObject();
  if (!config_object.HasMember("output_path")) {
    log.error("missing required field 'debugger.output_path' in configuration file at %s",
              path.c_str());
    return std::nullopt;
  }
  // OUTPUT PATH
  const auto output_path = config_object["output_path"].GetString();
  auto observer          = std::make_unique<Observer>(output_path);
  if (!config_object.HasMember("use_imu_manager")) {
    log.error("missing required field 'debugger.use_imu_manager' in configuration file at %s",
              path.c_str());
    return std::nullopt;
  }
  // USE IMU MANAGER? & IMU PINS
  const auto use_imu_manager = config_object["use_imu_manager"].GetBool();
  const auto imu_pin_vector  = sensors::Main::imuPinsFromFile(log, path);
  if (!imu_pin_vector) {
    log.error("failed to read IMU pins");
    return std::nullopt;
  }
  // FAKE TRAJECTORY?
  std::shared_ptr<sensors::FakeTrajectory> fake_trajectory;
  if (system.config_.use_fake_trajectory) {
    const auto fake_trajectory_optional = sensors::FakeTrajectory::fromFile(path);
    if (!fake_trajectory_optional) {
      log.error("failed to initialise fake trajectory");
      return std::nullopt;
    }
    fake_trajectory = std::make_shared<sensors::FakeTrajectory>(*fake_trajectory_optional);
  }
  // IMUs
  if (use_imu_manager) {
    if (fake_trajectory) {
      observer->addFakeImuManagerTask(fake_trajectory);
    } else {
      if (imu_pin_vector->size() != data::Sensors::kNumImus) {
        log.error("found %u imu pins but %u were expected", imu_pin_vector->size(),
                  data::Sensors::kNumImus);
        return std::nullopt;
      }
      sensors::ImuPins imu_pins;
      std::copy(imu_pin_vector->begin(), imu_pin_vector->end(), imu_pins.begin());
      observer->addImuManagerTask(imu_pins);
    }
  } else {  // use individual IMUs
    log.debug("adding imu tasks");
    for (const auto imu_pin : *imu_pin_vector) {
      observer->addImuTask(imu_pin);
    }
    if (fake_trajectory) {
      log.debug("adding fake imu tasks");
      auto fake_imus = sensors::FakeImu::fromFile(path, fake_trajectory);
      if (fake_imus) { observer->addFakeImuTasks(std::move(*fake_imus)); }
    }
  }
  // TEMPERATURE
  if (system.config_.use_fake_temperature) {
    observer->addFakeTemperatureTask(false);
  } else if (system.config_.use_fake_temperature_fail) {
    observer->addFakeTemperatureTask(true);
  } else {
    const auto pins
      = sensors::Main::ambientTemperaturePinsFromFile(log, system.config_.temperature_config_path);
    if (!pins) {
      log.error("failed to read temperature pin from file");
      return std::nullopt;
    }
    for (const auto pin : *pins) {
      observer->addTemperatureTask(pin);
    }
  }
  // BRAKE_PRESSURE
  if (system.config_.use_fake_brake_pressure) {
    observer->addFakeBrakePressureTask(false);
  } else if (system.config_.use_fake_brake_pressure_fail) {
    observer->addFakeBrakePressureTask(true);
  } else {
    const auto pins
      = sensors::Main::brakePressurePinsFromFile(log, system.config_.pressure_config_path);
    if (!pins) {
      log.error("failed to read brake pressure pin from file");
      return std::nullopt;
    }
    for (const auto pin : *pins) {
      observer->addBrakePressureTask(pin);
    }
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

void Observer::addFakeImuTasks(std::vector<std::unique_ptr<sensors::FakeImu>> fake_imus)
{
  uint32_t i = 0;
  char name_buffer[16];
  for (auto &fake_imu : fake_imus) {
    snprintf(name_buffer, 16, "fake_imu-%u", ++i);
    std::shared_ptr<sensors::FakeImu> fake_imu_ptr = std::move(fake_imu);
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

void Observer::addImuManagerTask(const sensors::ImuPins &imu_pins)
{
  auto imu_manager = std::make_shared<sensors::ImuManager>(imu_pins);
  imu_manager->start();
  Task imu_manager_task;
  imu_manager_task.name = "imu_manager";
  // The imu_manager needs to be captured even though it's not used to stop it from being
  // deallocated.
  imu_manager_task.handler = [imu_manager](JsonWriter &json_writer) {
    auto &data          = data::Data::getInstance();
    const auto imu_data = data.getSensorsImuData();
    json_writer.Key("timestamp");
    json_writer.Uint64(imu_data.timestamp);
    const auto imus = imu_data.value;
    json_writer.Key("imus");
    json_writer.StartArray();
    for (const auto &imu : imus) {
      json_writer.StartObject();
      json_writer.Key("operational");
      json_writer.Bool(imu.operational);
      json_writer.Key("acceleration");
      json_writer.StartArray();
      json_writer.Double(imu.acc[0]);
      json_writer.Double(imu.acc[1]);
      json_writer.Double(imu.acc[2]);
      json_writer.EndArray();
      json_writer.EndObject();
    }
    json_writer.EndArray();
  };
  tasks_.push_back(imu_manager_task);
}

void Observer::addFakeImuManagerTask(std::shared_ptr<sensors::FakeTrajectory> fake_trajectory)
{
  auto &system = utils::System::getSystem();
  std::shared_ptr<sensors::ImuManager> imu_manager
    = sensors::ImuManager::fromFile(system.config_.imu_config_path, fake_trajectory);
  imu_manager->start();
  Task imu_manager_task;
  imu_manager_task.name = "imu_manager";
  // The imu_manager needs to be captured even though it's not used to stop it from being
  // deallocated.
  imu_manager_task.handler = [imu_manager](JsonWriter &json_writer) {
    auto &data          = data::Data::getInstance();
    const auto imu_data = data.getSensorsImuData();
    json_writer.Key("timestamp");
    json_writer.Uint64(imu_data.timestamp);
    const auto imus = imu_data.value;
    json_writer.Key("imus");
    json_writer.StartArray();
    for (const auto &imu : imus) {
      json_writer.StartObject();
      json_writer.Key("operational");
      json_writer.Bool(imu.operational);
      json_writer.Key("acceleration");
      json_writer.StartArray();
      json_writer.Double(imu.acc[0]);
      json_writer.Double(imu.acc[1]);
      json_writer.Double(imu.acc[2]);
      json_writer.EndArray();
      json_writer.EndObject();
    }
    json_writer.EndArray();
  };
  tasks_.push_back(imu_manager_task);
}

void Observer::addTemperatureTask(const uint8_t pin)
{
  auto temperature = std::make_shared<sensors::AmbientTemperature>(pin);
  std::stringstream name;
  name << "temperature-" << static_cast<uint32_t>(pin);
  Task temperature_task;
  temperature_task.name    = name.str();
  temperature_task.handler = [temperature](JsonWriter &json_writer) {
    temperature->run();
    json_writer.Key("value");
    json_writer.Uint(temperature->getData());
  };
  tasks_.push_back(temperature_task);
}

void Observer::addFakeTemperatureTask(const bool is_fail)
{
  static uint32_t fake_temperature_id = 0;
  auto temperature                    = std::make_shared<sensors::FakeTemperature>(is_fail);
  std::stringstream name;
  name << "fake_temperature-" << fake_temperature_id++;
  Task temperature_task;
  temperature_task.name    = name.str();
  temperature_task.handler = [temperature](JsonWriter &json_writer) {
    temperature->run();
    json_writer.Key("value");
    json_writer.Uint(temperature->getData());
  };
  tasks_.push_back(temperature_task);
}

void Observer::addBrakePressureTask(const uint8_t pin)
{
  auto brake_pressure = std::make_shared<sensors::BrakePressure>(pin);
  std::stringstream name;
  name << "brake_pressure-" << static_cast<uint32_t>(pin);
  Task brake_pressure_task;
  brake_pressure_task.name    = name.str();
  brake_pressure_task.handler = [brake_pressure](JsonWriter &json_writer) {
    brake_pressure->run();
    json_writer.Key("value");
    json_writer.Uint(brake_pressure->getData());
  };
  tasks_.push_back(brake_pressure_task);
}

void Observer::addFakeBrakePressureTask(const bool is_fail)
{
  static uint32_t fake_brake_pressure_id = 0;
  auto brake_pressure                   = std::make_shared<sensors::FakeBrakePressure>(is_fail);
  std::stringstream name;
  name << "fake_brake_pressure-" << fake_brake_pressure_id++;
  Task brake_pressure_task;
  brake_pressure_task.name    = name.str();
  brake_pressure_task.handler = [brake_pressure](JsonWriter &json_writer) {
    brake_pressure->run();
    json_writer.Key("value");
    json_writer.Uint(brake_pressure->getData());
  };
  tasks_.push_back(brake_pressure_task);
}

}  // namespace hyped::debugging
