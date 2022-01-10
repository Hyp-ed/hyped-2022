#include "fake_keyence.hpp"

#include <fstream>
#include <random>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

namespace hyped::sensors {

FakeKeyence::FakeKeyence(const Config &config, std::shared_ptr<FakeTrajectory> fake_trajectory)
    : config_(config),
      data_(data::Data::getInstance()),
      fake_trajectory_(fake_trajectory)
{
  previous_data_.timestamp   = utils::Timer::getTimeMicros();
  previous_data_.value       = 0;  // start stripe count
  previous_data_.operational = true;
}

data::CounterData FakeKeyence::getData()
{
  const auto trajectory   = fake_trajectory_->getTrajectory();
  const auto displacement = addNoiseToDisplacement(trajectory.displacement);
  const auto implied_count
    = static_cast<uint32_t>(displacement / data::Navigation::kStripeDistance);
  if (previous_data_.value < implied_count) {
    previous_data_.timestamp = utils::Timer::getTimeMicros();
    previous_data_.value     = implied_count;
  }
  return previous_data_;
}

const FakeKeyence::Config &FakeKeyence::getConfig() const
{
  return config_;
}

std::optional<std::array<FakeKeyence, data::Sensors::kNumKeyence>> FakeKeyence::fromFile(
  utils::Logger &log, const std::string &path, std::shared_ptr<FakeTrajectory> fake_trajectory)
{
  const auto configs = readConfigs(log, path);
  if (!configs) {
    log.ERR("FAKE-KEYENCE", "Failed to read config at %s. Could not construct objects.",
            path.c_str());
    return std::nullopt;
  }
  return std::array<FakeKeyence, data::Sensors::kNumKeyence>(
    {FakeKeyence(configs->at(0), fake_trajectory), FakeKeyence(configs->at(1), fake_trajectory)});
}

std::optional<std::array<FakeKeyence::Config, data::Sensors::kNumKeyence>> FakeKeyence::readConfigs(
  utils::Logger &log, const std::string &path)
{
  std::ifstream input_stream(path);
  if (!input_stream.is_open()) {
    log.ERR("FAKE-KEYENCE", "Failed to open config file at %s", path.c_str());
    return std::nullopt;
  }
  rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
  rapidjson::Document document;
  document.ParseStream(input_stream_wrapper);
  if (document.HasParseError()) {
    log.ERR("FAKE-KEYENCE", "Failed to parse config file at %s", path.c_str());
    return std::nullopt;
  }
  if (!document.HasMember("fake_keyence")) {
    log.ERR("FAKE-KEYENCE", "Missing required field 'fake_keyence' in configuration file at %s",
            path.c_str());
    return std::nullopt;
  }
  auto config_object_array = document["fake_keyence"].GetArray();
  if (data::Sensors::kNumKeyence != config_object_array.Size()) {
    log.ERR("FAKE-KEYENCE",
            "Found %d config objects but %d were expected in configuration file at %s",
            config_object_array.Size(), data::Sensors::kNumKeyence, path.c_str());
    return std::nullopt;
  }
  std::array<FakeKeyence::Config, data::Sensors::kNumKeyence> configs;
  size_t i = 0;
  for (const auto &config_value : config_object_array) {
    const auto config_object = config_value.GetObject();
    FakeKeyence::Config config;
    if (!config_object.HasMember("noise")) {
      log.ERR("FAKE-KEYENCE",
              "Missing required field 'fake_keyence[%d].noise' in configuration file at %s", i,
              path.c_str());
      return std::nullopt;
    }
    config.noise = config_object["noise"].GetDouble();
    if (config_object.HasMember("failure_in_state")) {
      const auto state_name     = std::string(config_object["failure_in_state"].GetString());
      const auto state_optional = data::stateFromString(state_name);
      if (!state_optional) {
        log.ERR("FAKE-KEYENCE",
                "Unknown state name '%s' in field 'fake_keyence[%d].failure_in_state' in "
                "configuration file at %s",
                state_name.c_str(), i, path.c_str());
        return std::nullopt;
      }
      config.failure_in_state = state_optional;
    }
    configs.at(i) = config;
  }
  return configs;
}

data::nav_t FakeKeyence::addNoiseToDisplacement(const data::nav_t displacement) const
{
  static std::default_random_engine generator;
  std::normal_distribution<data::nav_t> distribution(displacement, config_.noise);
  return distribution(generator);
}

}  // namespace hyped::sensors
