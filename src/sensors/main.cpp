#include "main.hpp"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

#include <sensors/fake_temperature.hpp>
#include <sensors/temperature.hpp>

namespace hyped::sensors {

Main::Main()
    : utils::concurrent::Thread(
      utils::Logger("SENSORS", utils::System::getSystem().config_.log_level_sensors)),
      sys_(utils::System::getSystem()),
      data_(data::Data::getInstance())
{
  battery_manager_ = BmsManager::fromFile(sys_.config_.bms_config_path);
  if (!battery_manager_) {
    log_.error("failed to initialise bms");
    sys_.stop();
    return;
  }
  if (sys_.config_.use_fake_trajectory) {
    const auto fake_trajectory_optional
      = FakeTrajectory::fromFile(sys_.config_.fake_trajectory_config_path);
    if (!fake_trajectory_optional) {
      log_.error("failed to initialise fake trajectory");
      sys_.stop();
      return;
    }
    const auto fake_trajectory = std::make_shared<FakeTrajectory>(*fake_trajectory_optional);
    imu_manager_ = ImuManager::fromFile(sys_.config_.imu_config_path, fake_trajectory);
    if (!imu_manager_) {
      log_.error("failed to initialise fake imus");
      sys_.stop();
      return;
    }
  } else {
    // Real trajectory sensors
    auto imu_pin_vector = imuPinsFromFile(log_, sys_.config_.imu_config_path);
    if (!imu_pin_vector) {
      log_.error("failed to initialise IMUs");
      sys_.stop();
      return;
    }
    ImuPins imu_pins;
    std::copy(imu_pin_vector->begin(), imu_pin_vector->end(), imu_pins.begin());
    imu_manager_ = std::make_unique<ImuManager>(imu_pins);
    if (!imu_manager_) {
      log_.error("failed to initialise imus");
      sys_.stop();
      return;
    }
  }

  // Temperature
  if (sys_.config_.use_fake_temperature_fail) {
    temperature_ = std::make_unique<FakeTemperature>(log_, true);
  } else if (sys_.config_.use_fake_temperature) {
    temperature_ = std::make_unique<FakeTemperature>(log_, false);
  } else {
    auto temperature_pin = temperaturePinFromFile(log_, sys_.config_.temperature_config_path);
    if (!temperature_pin) {
      log_.error("failed to initialise temperature sensor");
      sys_.stop();
      return;
    }
    temperature_ = std::make_unique<Temperature>(log_, *temperature_pin);
  }

  // kReady for state machine transition
  sensors_               = data_.getSensorsData();
  sensors_.module_status = data::ModuleStatus::kReady;
  data_.setSensorsData(sensors_);
  log_.info("Sensors have been initialised");
}

void Main::checkTemperature()
{
  temperature_->run();  // not a thread
  data_.setTemperature(temperature_->getData());
  if (data_.getTemperature() > 85 && !log_error_) {
    log_.info("PCB temperature is getting a wee high...sorry Cheng");
    log_error_ = true;
  }
}

std::optional<std::vector<uint8_t>> Main::imuPinsFromFile(utils::Logger &log,
                                                          const std::string &path)
{
  std::ifstream input_stream(path);
  if (!input_stream.is_open()) {
    log.error("Failed to open config file at %s", path.c_str());
    return std::nullopt;
  }
  rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
  rapidjson::Document document;
  document.ParseStream(input_stream_wrapper);
  if (document.HasParseError()) {
    log.error("Failed to parse config file at %s", path.c_str());
    return std::nullopt;
  }
  if (!document.HasMember("sensors")) {
    log.error("Missing required field 'sensors' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  auto config_object = document["sensors"].GetObject();
  if (!config_object.HasMember("imu_pins")) {
    log.error("Missing required field 'sensors.imu_pins' in configuration file at %s",
              path.c_str());
    return std::nullopt;
  }
  auto imu_pin_array = config_object["imu_pins"].GetArray();
  std::vector<uint8_t> imu_pins;
  for (auto &imu_pin : imu_pin_array) {
    imu_pins.push_back(static_cast<uint8_t>(imu_pin.GetUint()));
  }
  return imu_pins;
}

std::optional<std::uint32_t> Main::temperaturePinFromFile(utils::Logger &log,
                                                          const std::string &path)
{
  std::ifstream input_stream(path);
  if (!input_stream.is_open()) {
    log.error("Failed to open config file at %s", path.c_str());
    return std::nullopt;
  }
  rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
  rapidjson::Document document;
  document.ParseStream(input_stream_wrapper);
  if (document.HasParseError()) {
    log.error("Failed to parse config file at %s", path.c_str());
    return std::nullopt;
  }
  if (!document.HasMember("sensors")) {
    log.error("Missing required field 'sensors' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  auto config_object = document["sensors"].GetObject();
  if (!config_object.HasMember("temperature_pin")) {
    log.error("Missing required field 'sensors.temperature_pin' in configuration file at %s",
              path.c_str());
    return std::nullopt;
  }
  return static_cast<std::uint32_t>(config_object["temperature_pin"].GetUint());
}

void Main::run()
{
  battery_manager_->start();
  imu_manager_->start();

  size_t temp_count = 0;
  while (sys_.isRunning()) {
    Thread::sleep(10);
    ++temp_count;
    // only check every 20 cycles
    if (temp_count % 20 == 0) {
      checkTemperature();
      // avoid overflow
      temp_count = 0;
    }
  }
  imu_manager_->join();
  battery_manager_->join();
}
}  // namespace hyped::sensors
