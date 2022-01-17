#include "fake_wheel_encoder.hpp"

#include <fstream>
#include <random>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
namespace hyped::sensors {

FakeWheelEncoder::FakeWheelEncoder(const Config &config,
                                   std::shared_ptr<FakeTrajectory> fake_trajectory)
    : config_(config),
      data_(data::Data::getInstance()),
      fake_trajectory_(fake_trajectory)
{
  internal_data_.timestamp   = utils::Timer::getTimeMicros();
  internal_data_.value       = 0;  // start stripe count
  internal_data_.operational = true;
}

data::CounterData FakeWheelEncoder::getData()
{
  if (config_.failure_in_state) {
    const auto current_state = data_.getStateMachineData().current_state;
    if (current_state == *config_.failure_in_state) { internal_data_.operational = false; }
  }
  if (!internal_data_.operational) {
    internal_data_.timestamp = utils::Timer::getTimeMicros();
    // Randomise data to make sure this is not being used anywhere
    internal_data_.value = static_cast<uint32_t>(std::rand());
    return internal_data_;
  }
  // No failure (...yet)
  const auto trajectory   = fake_trajectory_->getTrajectory();
  const auto displacement = addNoiseToDisplacement(trajectory.displacement);
  const auto implied_count
    = static_cast<uint32_t>(displacement / data::Navigation::kWheelCircumfrence);
  if (internal_data_.value < implied_count) {
    internal_data_.timestamp = utils::Timer::getTimeMicros();
    internal_data_.value     = implied_count;
  }
  return internal_data_;
}

const FakeWheelEncoder::Config &FakeWheelEncoder::getConfig() const
{
  return config_;
}

std::optional<std::array<FakeWheelEncoder, data::Sensors::kNumEncoders>> FakeWheelEncoder::fromFile(
  utils::Logger &log, const std::string &path, std::shared_ptr<FakeTrajectory> fake_trajectory)
{
  const auto configs = readConfigs(log, path);
  if (!configs) {
    log.ERR("FAKE-WHEEL-ENCODER", "Failed to read config at %s. Could not construct objects.",
            path.c_str());
    return std::nullopt;
  }
  return std::array<FakeWheelEncoder, data::Sensors::kNumEncoders>({
    FakeWheelEncoder(configs->at(0), fake_trajectory),
    FakeWheelEncoder(configs->at(1), fake_trajectory),
    FakeWheelEncoder(configs->at(2), fake_trajectory),
    FakeWheelEncoder(configs->at(3), fake_trajectory),
  });
}

std::optional<std::array<FakeWheelEncoder::Config, data::Sensors::kNumEncoders>>
  FakeWheelEncoder::readConfigs(utils::Logger &log, const std::string &path)
{
  std::ifstream input_stream(path);
  if (!input_stream.is_open()) {
    log.ERR("FAKE-WHEEL-ENCODER", "Failed to open config file at %s", path.c_str());
    return std::nullopt;
  }
  rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
  rapidjson::Document document;
  document.ParseStream(input_stream_wrapper);
  if (document.HasParseError()) {
    log.ERR("FAKE-WHEEL-ENCODER", "Failed to parse config file at %s", path.c_str());
    return std::nullopt;
  }
  if (!document.HasMember("fake_wheel_encoder")) {
    log.ERR("FAKE-WHEEL-ENCODER",
            "Missing required field 'fake_wheel_encoder' in configuration file at %s",
            path.c_str());
    return std::nullopt;
  }
  auto config_object_array = document["fake_wheel_encoder"].GetArray();
  if (data::Sensors::kNumEncoders != config_object_array.Size()) {
    log.ERR("FAKE-WHEEL-ENCODER",
            "Found %d config objects but %d were expected in configuration file at %s",
            config_object_array.Size(), data::Sensors::kNumEncoders, path.c_str());
    return std::nullopt;
  }
  std::array<FakeWheelEncoder::Config, data::Sensors::kNumEncoders> configs;
  size_t i = 0;
  for (const auto &config_value : config_object_array) {
    const auto config_object = config_value.GetObject();
    FakeWheelEncoder::Config config;
    if (!config_object.HasMember("noise")) {
      log.ERR("FAKE-WHEEL-ENCODER",
              "Missing required field 'fake_wheel_encoder[%d].noise' in configuration file at %s",
              i, path.c_str());
      return std::nullopt;
    }
    config.noise = static_cast<data::nav_t>(config_object["noise"].GetDouble());
    if (config_object.HasMember("failure_in_state")) {
      const auto state_name     = std::string(config_object["failure_in_state"].GetString());
      const auto state_optional = data::stateFromString(state_name);
      if (!state_optional) {
        log.ERR("FAKE-WHEEL-ENCODER",
                "Unknown state name '%s' in field 'fake_wheel_encoder[%d].failure_in_state' in "
                "configuration file at %s",
                state_name, i, path.c_str());
        return std::nullopt;
      }
      config.failure_in_state = state_optional;
    }
    configs.at(i) = config;
    ++i;
  }
  return configs;
}

data::nav_t FakeWheelEncoder::addNoiseToDisplacement(const data::nav_t displacement) const
{
  static std::default_random_engine generator;
  std::normal_distribution<data::nav_t> distribution(displacement, config_.noise);
  return distribution(generator);
}

}  // namespace hyped::sensors
