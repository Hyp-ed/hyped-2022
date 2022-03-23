#include "main.hpp"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

#include <sensors/fake_keyence.hpp>
#include <sensors/fake_temperature.hpp>
#include <sensors/gpio_counter.hpp>
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
    const auto fake_keyences_optional
      = FakeKeyence::fromFile(sys_.config_.keyence_config_path, fake_trajectory);
    if (!fake_keyences_optional) {
      log_.error("failed to initialise fake keyence");
      sys_.stop();
      return;
    }
    for (size_t i = 0; i < data::Sensors::kNumKeyence; ++i) {
      keyences_.at(i) = std::make_unique<FakeKeyence>(fake_keyences_optional->at(i));
    }
    imu_manager_ = ImuManager::fromFile(sys_.config_.imu_config_path, fake_trajectory);
    if (!imu_manager_) {
      log_.error("failed to initialise fake imus");
      sys_.stop();
      return;
    }
  } else {
    // Real trajectory sensors
    auto keyence_pins = keyencePinsFromFile(log_, sys_.config_.keyence_config_path);
    if (!keyence_pins) {
      log_.error("failed to initialise keyence");
      sys_.stop();
      return;
    }
    for (size_t i = 0; i < data::Sensors::kNumKeyence; ++i) {
      auto keyence = std::make_unique<GpioCounter>(keyence_pins->at(i));
      keyence->start();
      keyences_[i] = std::move(keyence);
    }
    auto imu_pins = imuPinsFromFile(log_, sys_.config_.imu_config_path);
    if (!imu_pins) {
      log_.error("failed to initialise IMUs");
      sys_.stop();
      return;
    }
    imu_manager_ = std::make_unique<ImuManager>(
      utils::Logger("IMU-MANAGER", sys_.config_.log_level_sensors), *imu_pins);
    if (!imu_manager_) {
      log_.error("failed to initialise imus");
      sys_.stop();
      return;
    }
  }

  // Temperature
  if (sys_.config_.use_fake_temperature_fail) {
    // temperature_ = std::make_unique<FakeTemperature>(true);
    for (size_t i = 0; i < data::Sensors::kNumAmbientTemp; ++i) {
      temperature_[i] = std::make_unique<FakeTemperature>(true);
    }
  } else if (sys_.config_.use_fake_temperature) {
    for (size_t i = 0; i < data::Sensors::kNumAmbientTemp; ++i) {
      temperature_[i] = std::make_unique<FakeTemperature>(false);
    }
  } else {
    auto temperature_pins
      = ambientTemperaturePinsFromFile(log_, sys_.config_.temperature_config_path);
    if (!temperature_pins) {
      log_.error("failed to initialise temperature sensor");
      sys_.stop();
      return;
    }
    for (size_t i = 0; i < data::Sensors::kNumAmbientTemp; ++i) {
      temperature_[i] = std::make_unique<Temperature>(*temperature_pins);
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
  auto ambient_temperature = data_.getSensorsData().ambient_temperature_array;
  for (size_t i = 0; i < data::Sensors::kNumAmbientTemp; ++i) {
    temperature_[i]->run();  // not a thread
    //.temp is maybe not the nicest method...
    ambient_temperature.at(i).temp = temperature_[i]->getData();
    if ((ambient_temperature.at(i).temp > 75UL || ambient_temperature.at(i).temp < 1UL)
        && !log_error_) {  // 85 is the critical temperature so we alert at 75
      log_.info("PCB temperature is getting a wee high...sorry Cheng");
      log_error_ = true;
    }
  }
}

/**
void Main::checkBrakeTemperature()
// TODO: edit this so that it checks EACH of the AMBIENT sensors are within the critical limit.
{
  auto brake_temperature = data_.getSensorsData().brake_temperature_array;
  for (size_t i = 0; i < data::Sensors::kNumAmbientTemp; ++i) {
    temperature_[i]->run();  // not a thread
    brake_temperature.at(i) = temperature_[i]->getData();
    // how do we differentiate here between the different runs - i.e. call the different temperature
    // sensors?
    // uint8_t converted_temperature = temperature_->getData();
    if ((brake_temperature.at(i) > 75UL || brake_temperature.at(i) < 1UL)
        && !log_error_) {  // 85 is the critical temperature so we alert at 75
      log_.info("PCB temperature is getting a wee high...sorry Cheng");
      log_error_ = true;
    }
  }
}
*/

void Main::checkPressure()
{
  pressure_->run();  // not a thread

  uint8_t converted_pressure = pressure_->getData();
  if (converted_pressure > 880 && !log_error_) {  // upper threshold is 900 is 880 as a safety
    log_.info("PCB pressure is too high!");
    log_error_ = true;
  }
  if (converted_pressure < 560 && !log_error_) {
    log_.info("PCB pressure has dropped too low!");  // lower threshold is 540 so 560 as a safety
    log_error_ = true;
  }
}

std::optional<Main::KeyencePins> Main::keyencePinsFromFile(utils::Logger &log,
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
  if (!config_object.HasMember("keyence_pins")) {
    log.error("Missing required field 'sensors.keyence_pins' in configuration file at %s",
              path.c_str());
    return std::nullopt;
  }
  auto keyence_pin_array = config_object["keyence_pins"].GetArray();
  if (keyence_pin_array.Size() != data::Sensors::kNumKeyence) {
    log.error("Found %d keyence pins but %d were expected in configuration file at %s",
              keyence_pin_array.Size(), data::Sensors::kNumKeyence, path.c_str());
  }
  KeyencePins keyence_pins;
  std::size_t i = 0;
  for (auto &keyence_pin : keyence_pin_array) {
    keyence_pins.at(i) = static_cast<uint32_t>(keyence_pin.GetUint());
    ++i;
  }
  return keyence_pins;
}

std::optional<Main::ImuPins> Main::imuPinsFromFile(utils::Logger &log, const std::string &path)
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
  if (imu_pin_array.Size() != data::Sensors::kNumImus) {
    log.error("Found %d keyence pins but %d were expected in configuration file at %s",
              imu_pin_array.Size(), data::Sensors::kNumImus, path.c_str());
  }
  ImuPins imu_pins;
  std::size_t i = 0;
  for (auto &imu_pin : imu_pin_array) {
    imu_pins.at(i) = static_cast<uint32_t>(imu_pin.GetUint());
    ++i;
  }
  return imu_pins;
}

std::optional<Main::AmbientTemperaturePins> Main::ambientTemperaturePinsFromFile(
  utils::Logger &log, const std::string &path)
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
  auto ambient_temperature_pin_array = config_object["temperature_pins"].GetArray();
  if (ambient_temperature_pin_array.Size() != data::Sensors::kNumAmbientTemp) {
    log.error("Found %d ambient temperature pins but %d were expected in configuration file at %s",
              ambient_temperature_pin_array.Size(), data::Sensors::kNumAmbientTemp, path.c_str());
  }
  AmbientTemperaturePins ambient_temperature_pins;
  std::size_t i = 0;
  for (auto &ambient_temperature_pin : ambient_temperature_pin_array) {
    ambient_temperature_pins.at(i) = static_cast<uint32_t>(ambient_temperature_pin.GetUint());
    ++i;
  }
  return ambient_temperature_pins;
}

void Main::run()
{
  battery_manager_->start();
  imu_manager_->start();

  auto current_keyence  = data_.getSensorsKeyenceData();
  auto previous_keyence = current_keyence;

  // Intialise temperature and pressure
  auto brake_temperature = data_.getSensorsData().brake_temperature_array;
  pressure_data_         = data_.getSensorsData().pressure;

  std::size_t iteration_count = 0;
  while (sys_.isRunning()) {
    bool keyence_updated = false;
    for (size_t i = 0; i < current_keyence.size(); ++i) {
      if (current_keyence.at(i).timestamp > previous_keyence.at(i).timestamp) {
        keyence_updated = true;
        break;
      }
    }
    if (keyence_updated) {
      data_.setSensorsKeyenceData(current_keyence);
      previous_keyence = current_keyence;
    }
    for (size_t i = 0; i < data::Sensors::kNumKeyence; ++i) {
      current_keyence.at(i) = keyences_[i]->getData();
    }
    Thread::sleep(10);  // Sleep for 10ms
    ++iteration_count;
    if (iteration_count % 20 == 0) {  // check every 20 cycles of main
      checkAmbientTemperature();
      checkPressure();
      // So that temp_count does not get huge
      iteration_count = 0;
    }
  }
  imu_manager_->join();
  battery_manager_->join();
}
}  // namespace hyped::sensors
