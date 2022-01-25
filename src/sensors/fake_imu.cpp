#include "fake_imu.hpp"

#include <fstream>
#include <random>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

namespace hyped::sensors {

FakeImu::FakeImu(const Config &config, std::shared_ptr<FakeTrajectory> fake_trajectory)
    : config_(config),
      data_(data::Data::getInstance()),
      fake_trajectory_(fake_trajectory),
      is_operational_(true)
{
}

data::NavigationVector FakeImu::getAccurateAcceleration()
{
  const auto trajectory = fake_trajectory_->getTrajectory();
  data::NavigationVector value;
  value[0] = trajectory.acceleration;
  value[1] = 0.0;
  value[2] = 9.8;
  return value;
}

data::ImuData FakeImu::getData()
{
  if (config_.failure_in_state) {
    const auto current_state = data_.getStateMachineData().current_state;
    if (current_state == *config_.failure_in_state) { is_operational_ = false; }
  }
  data::ImuData imu_data;
  imu_data.operational = is_operational_;
  if (is_operational_) {
    imu_data.acc = addNoiseToAcceleration(getAccurateAcceleration());
  } else {
    static std::default_random_engine generator;
    std::uniform_real_distribution<data::nav_t> distribution;
    for (size_t i = 0; i < 3; ++i) {
      imu_data.acc[i] = distribution(generator);
    }
  }
  return imu_data;
}

const FakeImu::Config &FakeImu::getConfig() const
{
  return config_;
}

std::optional<std::array<FakeImu, data::Sensors::kNumImus>> FakeImu::fromFile(
  utils::Logger &log, const std::string &path, std::shared_ptr<FakeTrajectory> fake_trajectory)
{
  const auto configs = readConfigs(log, path);
  if (!configs) {
    log.error("Failed to read config at %s. Could not construct objects.", path.c_str());
    return std::nullopt;
  }
  return std::array<FakeImu, data::Sensors::kNumImus>({
    FakeImu(configs->at(0), fake_trajectory),
    FakeImu(configs->at(1), fake_trajectory),
    FakeImu(configs->at(2), fake_trajectory),
    FakeImu(configs->at(3), fake_trajectory),
  });
}

std::optional<std::array<FakeImu::Config, data::Sensors::kNumImus>> FakeImu::readConfigs(
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
  if (!document.HasMember("fake_imu")) {
    log.error("Missing required field 'fake_imu' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  auto config_object_array = document["fake_imu"].GetArray();
  if (data::Sensors::kNumImus != config_object_array.Size()) {
    log.error("Found %d config objects but %d were expected in configuration file at %s",
              config_object_array.Size(), data::Sensors::kNumImus, path.c_str());
    return std::nullopt;
  }
  std::array<FakeImu::Config, data::Sensors::kNumImus> configs;
  size_t i = 0;
  for (const auto &config_value : config_object_array) {
    const auto config_object = config_value.GetObject();
    FakeImu::Config config;
    if (!config_object.HasMember("noise")) {
      log.error("Missing required field 'fake_imu[%d].noise' in configuration file at %d", i,
                path.c_str());
      return std::nullopt;
    }
    config.noise = static_cast<data::nav_t>(config_object["noise"].GetDouble());
    if (config_object.HasMember("failure_in_state")) {
      const auto state_name     = std::string(config_object["failure_in_state"].GetString());
      const auto state_optional = data::stateFromString(state_name);
      if (!state_optional) {
        log.error(
          "Unknown state name '%s' in field 'fake_imu[%d].failure_in_state' in configuration "
          "file at %s",
          state_name.c_str(), i, path.c_str());
        return std::nullopt;
      }
      config.failure_in_state = state_optional;
    }
    configs.at(i) = config;
    ++i;
  }
  return configs;
}

data::NavigationVector FakeImu::addNoiseToAcceleration(
  const data::NavigationVector acceleration) const
{
  data::NavigationVector temp;
  static std::default_random_engine generator;

  for (size_t i = 0; i < 3; ++i) {
    std::normal_distribution<data::nav_t> distribution(acceleration[i], config_.noise);
    temp[i] = distribution(generator);
  }
  return temp;
}

}  // namespace hyped::sensors
