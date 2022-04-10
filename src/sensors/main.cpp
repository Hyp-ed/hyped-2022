#include "main.hpp"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

#include <sensors/ambient_pressure.hpp>
#include <sensors/brake_pressure.hpp>
#include <sensors/fake_ambient_pressure.hpp>
#include <sensors/fake_brake_pressure.hpp>
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
    for (auto &ambient_temperature : ambient_temperatures_) {
      ambient_temperature = std::make_unique<FakeTemperature>(true);
    }
  } else if (sys_.config_.use_fake_temperature) {
    for (auto &ambient_temperature : ambient_temperatures_) {
      ambient_temperature = std::make_unique<FakeTemperature>(false);
    }
  } else {
    const auto ambient_temperature_pins
      = ambientTemperaturePinsFromFile(log_, sys_.config_.temperature_config_path);
    if (!ambient_temperature_pins) {
      log_.error("failed to initialise temperature sensor");
      sys_.stop();
      return;
    }
    if (ambient_temperature_pins->size() != data::Sensors::kNumAmbientTemp) {
      log_.error("found %u temperature pins but %u were expected",
                 static_cast<uint32_t>(ambient_temperature_pins->size()),
                 static_cast<uint32_t>(data::Sensors::kNumAmbientTemp));
      sys_.stop();
      return;
    }
    for (size_t i = 0; i < data::Sensors::kNumAmbientTemp; ++i) {
      ambient_temperatures_.at(i) = std::make_unique<Temperature>(ambient_temperature_pins->at(i));
    }
  }

  // AmbientPressure
  if (sys_.config_.use_fake_ambient_pressure_fail) {
    ambient_pressure_ = std::make_unique<FakeAmbientPressure>(true);
  } else if (sys_.config_.use_fake_ambient_pressure) {
    ambient_pressure_ = std::make_unique<FakeAmbientPressure>(false);
  } else {
    const auto ambient_pressure_pins
      = ambientPressurePinsFromFile(log_, sys_.config_.pressure_config_path);
    if (!ambient_pressure_pins) {
      log_.error("failed to initialise ambient pressure sensor");
      sys_.stop();
      return;
    }
    ambient_pressure_ = std::make_unique<AmbientPressure>(ambient_pressure_pins->pressure_pin,
                                                          ambient_pressure_pins->temperature_pin);
  }

  // BrakePressure
  if (sys_.config_.use_fake_brake_pressure_fail) {
    for (size_t i = 0; i < data::Sensors::kNumBrakePressure; ++i) {
      brake_pressures_.at(i) = std::make_unique<FakeBrakePressure>(true);
    }
  } else if (sys_.config_.use_fake_brake_pressure) {
    for (size_t i = 0; i < data::Sensors::kNumBrakePressure; ++i) {
      brake_pressures_.at(i) = std::make_unique<FakeBrakePressure>(false);
    }
  } else {
    const auto brake_pressure_pins
      = brakePressurePinsFromFile(log_, sys_.config_.pressure_config_path);
    if (!brake_pressure_pins) {
      log_.error("failed to initialise brake pressure sensor");
      sys_.stop();
      return;
    }
    for (size_t i = 0; i < data::Sensors::kNumBrakePressure; ++i) {
      brake_pressures_.at(i) = std::make_unique<BrakePressure>(brake_pressure_pins->at(i));
    }
  }
  // kReady for state machine transition
  sensors_               = data_.getSensorsData();
  sensors_.module_status = data::ModuleStatus::kReady;
  data_.setSensorsData(sensors_);
  log_.info("Sensors have been initialised");
}

void Main::checkAmbientTemperature()
// TODO: edit this so that it checks EACH of the AMBIENT sensors are within the critical limit.
{
  auto ambient_temperature_data = data_.getSensorsData().ambient_temperature_array;
  for (size_t i = 0; i < data::Sensors::kNumAmbientTemp; ++i) {
    ambient_temperatures_.at(i)->run();  // not a thread
    ambient_temperature_data.at(i).temperature = ambient_temperatures_[i]->getData();
    if ((ambient_temperature_data.at(i).temperature > 75
         || ambient_temperature_data.at(i).temperature < 1)) {
      log_.info("PCB temperature is getting a wee high...sorry Cheng");
      auto sensors_data          = data_.getSensorsData();
      sensors_data.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setSensorsData(sensors_data);
    }
  }
}

void Main::checkBrakeTemperature()
{
  auto brakes_temperature_data = data_.getSensorsData().brake_temperature_array;
  for (size_t i = 0; i < data::Sensors::kNumBrakeTemp; ++i) {
    brake_temperatures_[i]->run();  // not a thread
    brakes_temperature_data.at(i).temperature = brake_temperatures_[i]->getData();
    if ((brakes_temperature_data.at(i).temperature > 75
         || brakes_temperature_data.at(i).temperature < 0)) {
      log_.info("PCB temperature is getting a wee high...sorry Cheng");
      auto sensors_data          = data_.getSensorsData();
      sensors_data.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setSensorsData(sensors_data);
    }
  }
}

void Main::checkAmbientPressure()
{
  ambient_pressure_->run();                                    // not a thread
  const auto ambient_pressure = ambient_pressure_->getData();  // mbar
  if (ambient_pressure > 1200) {
    log_.info("Ambient pressure (%u) exceeds maximum value (%d)", ambient_pressure, 1200);
    auto sensors_data          = data_.getSensorsData();
    sensors_data.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setSensorsData(sensors_data);
  }
}

void Main::checkBrakePressure()
{
  for (size_t i = 0; i < brake_pressures_.size(); ++i) {
    brake_pressures_[i]->run();
    const auto brake_pressure = brake_pressures_[i]->getData();
    if (brake_pressure > 9000) {
      log_.info("Brake pressure (%u) exceeds maximum value (%d)", brake_pressure, 9000);
      auto sensors_data          = data_.getSensorsData();
      sensors_data.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setSensorsData(sensors_data);
    } else if (brake_pressure < 5600) {
      log_.info("Brake pressure (%u) exceeds minimum value (%d)", brake_pressure, 5600);
      auto sensors_data          = data_.getSensorsData();
      sensors_data.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setSensorsData(sensors_data);
    }
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
  const auto config_object = document["sensors"].GetObject();
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

std::optional<std::vector<uint8_t>> Main::ambientTemperaturePinsFromFile(utils::Logger &log,
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
  if (!config_object.HasMember("temperature_pins")) {
    log.error("Missing required field 'sensors.temperature_pins' in configuration file at %s",
              path.c_str());
    return std::nullopt;
  }
  const auto ambient_temperature_pin_array = config_object["temperature_pins"].GetArray();
  if (ambient_temperature_pin_array.Size() != data::Sensors::kNumAmbientTemp) {
    log.error("Found %d ambient temperature pins but %d were expected in configuration file at %s",
              ambient_temperature_pin_array.Size(), data::Sensors::kNumAmbientTemp, path.c_str());
  }
  std::vector<uint8_t> ambient_temperature_pins;
  for (const auto &ambient_temperature_pin : ambient_temperature_pin_array) {
    const auto pin = ambient_temperature_pin.GetUint();
    if (pin > UINT8_MAX) {
      log.error("ambient temperature pin value %u is too large", pin);
      return std::nullopt;
    }
    ambient_temperature_pins.push_back(static_cast<uint8_t>(ambient_temperature_pin.GetUint()));
  }
  return ambient_temperature_pins;
}

std::optional<std::vector<uint8_t>> Main::brakeTemperaturePinsFromFile(utils::Logger &log,
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
  if (!config_object.HasMember("temperature_pins")) {
    log.error("Missing required field 'sensors.temperature_pins' in configuration file at %s",
              path.c_str());
    return std::nullopt;
  }
  const auto brake_temperature_pin_array = config_object["temperature_pins"].GetArray();
  if (brake_temperature_pin_array.Size() != data::Sensors::kNumAmbientTemp) {
    log.error("Found %d brake temperature pins but %d were expected in configuration file at %s",
              brake_temperature_pin_array.Size(), data::Sensors::kNumBrakeTemp, path.c_str());
  }
  std::vector<uint8_t> brake_temperature_pins;
  for (const auto &brake_temperature_pin : brake_temperature_pin_array) {
    const auto pin = brake_temperature_pin.GetUint();
    if (pin > UINT8_MAX) {
      log.error("brake temperature pin value %u is too large", pin);
      return std::nullopt;
    }
    brake_temperature_pins.push_back(static_cast<uint8_t>(pin));
  }
  return brake_temperature_pins;
}

std::optional<AmbientPressurePins> Main::ambientPressurePinsFromFile(utils::Logger &log,
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
  const auto config_object = document["sensors"].GetObject();
  if (!config_object.HasMember("ambient_pressure_pins")) {
    log.error("Missing required field 'sensors.ambient_pressure_pins' in configuration file at %s",
              path.c_str());
    return std::nullopt;
  }
  const auto ambient_pressure_pins_object = config_object["ambient_pressure_pins"].GetObject();
  AmbientPressurePins ambient_pressure_pins;
  if (!ambient_pressure_pins_object.HasMember("pressure_pin")) {
    log.error(
      "Missing required field 'sensors.ambient_pressure_pins.pressure_pin' in configuration file "
      "at %s",
      path.c_str());
    return std::nullopt;
  }
  ambient_pressure_pins.pressure_pin = ambient_pressure_pins_object["pressure_pin"].GetUint();
  if (!ambient_pressure_pins_object.HasMember("temperature_pin")) {
    log.error(
      "Missing required field 'sensors.ambient_pressure_pins.temperature_pin' in configuration "
      "file at %s",
      path.c_str());
    return std::nullopt;
  }
  ambient_pressure_pins.temperature_pin = ambient_pressure_pins_object["temperature_pin"].GetUint();
  return ambient_pressure_pins;
}

std::optional<std::vector<uint8_t>> Main::brakePressurePinsFromFile(utils::Logger &log,
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
  const auto config_object = document["sensors"].GetObject();
  if (!config_object.HasMember("brake_pressure_pins")) {
    log.error("Missing required field 'sensors.brake_pressure_pins' in configuration file at %s",
              path.c_str());
    return std::nullopt;
  }
  const auto brake_pressure_pin_array = config_object["brake_pressure_pins"].GetArray();
  if (brake_pressure_pin_array.Size() != data::Sensors::kNumBrakePressure) {
    log.error("Found %d brake sensor pins but %d were expected in configuration file at %s",
              brake_pressure_pin_array.Size(), data::Sensors::kNumBrakePressure, path.c_str());
  }
  std::vector<uint8_t> brake_pressure_pins;
  std::size_t i = 0;
  for (auto &brake_pressure_pin : brake_pressure_pin_array) {
    brake_pressure_pins.at(i) = static_cast<uint32_t>(brake_pressure_pin.GetUint());
    ++i;
  }
  return brake_pressure_pins;
}

void Main::run()
{
  battery_manager_->start();
  imu_manager_->start();

  while (sys_.isRunning()) {
    checkAmbientTemperature();
    checkAmbientPressure();
    checkBrakePressure();
    Thread::sleep(200);
  }

  imu_manager_->join();
  battery_manager_->join();
}
}  // namespace hyped::sensors
