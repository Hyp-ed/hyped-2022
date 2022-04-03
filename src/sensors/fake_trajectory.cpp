#include "fake_trajectory.hpp"

#include <fstream>
#include <random>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

#include <utils/system.hpp>

namespace hyped::sensors {

std::optional<FakeTrajectory::Config> FakeTrajectory::readConfig(utils::Logger &log,
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
  if (!document.HasMember("fake_trajectory")) {
    log.error("Missing required field 'fake_trajectory' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  const auto config_object = document["fake_trajectory"].GetObject();
  FakeTrajectory::Config config;
  if (!config_object.HasMember("maximum_acceleration")) {
    log.error(
      "Missing required field 'fake_trajectory.maximum_acceleration' in configuration file at %s",
      path.c_str());
    return std::nullopt;
  }
  config.maximum_acceleration = config_object["maximum_acceleration"].GetDouble();
  if (!config_object.HasMember("braking_deceleration")) {
    log.error(
      "Missing required field 'fake_trajectory.braking_deceleration' in configuration file at %s",
      path.c_str());
    return std::nullopt;
  }
  config.braking_deceleration = config_object["braking_deceleration"].GetDouble();
  if (!config_object.HasMember("cruising_deceleration")) {
    log.error(
      "Missing required field 'fake_trajectory.cruising_deceleration' in configuration file at %s",
      path.c_str());
    return std::nullopt;
  }
  config.cruising_deceleration = config_object["cruising_deceleration"].GetDouble();
  return config;
}

std::optional<FakeTrajectory> FakeTrajectory::fromFile(const std::string &path)
{
  auto &system = utils::System::getSystem();
  utils::Logger log("FAKE-TRAJECTORY", system.config_.log_level);
  const auto config_optional = readConfig(log, path);
  if (!config_optional) {
    log.error("Failed to read config at %s. Could not construct object.", path.c_str());
    return std::nullopt;
  }
  return FakeTrajectory(*config_optional);
}

FakeTrajectory::FakeTrajectory(const Config &config)
    : config_(config),
      data_(data::Data::getInstance()),
      last_update_(utils::Timer::getTimeMicros()),
      trajectory_({0.0, 0.0, 0.0})
{
}

FakeTrajectory::Trajectory FakeTrajectory::getTrajectory()
{
  const auto stm_data              = data_.getStateMachineData();
  const uint64_t now               = utils::Timer::getTimeMicros();
  const data::nav_t seconds_passed = static_cast<double>(now - last_update_) / 1e6;
  last_update_                     = now;
  switch (stm_data.current_state) {
    case data::State::kIdle:
    case data::State::kPreCalibrating:
    case data::State::kCalibrating:
    case data::State::kPreReady:
    case data::State::kReady:
    case data::State::kFailureStopped:
    case data::State::kFinished:
    case data::State::kInvalid:
      trajectory_.acceleration = 0.0;
      trajectory_.velocity     = 0.0;
      return trajectory_;
    case data::State::kAccelerating:
      trajectory_.acceleration = config_.maximum_acceleration;
      break;
    case data::State::kCruising:
      // We assume acceleration is off and brakes are not engaged in pre-braking states:
    case data::State::kPreBraking:
    case data::State::kFailurePreBraking:
      trajectory_.acceleration = -config_.cruising_deceleration;
      break;
    case data::State::kNominalBraking:
    case data::State::kFailureBraking:
      trajectory_.acceleration = -config_.braking_deceleration;
      break;
  }
  // s = s0 + v0 * dt + 1/2 * a0 * dt^2
  trajectory_.displacement += trajectory_.velocity * seconds_passed;
  trajectory_.displacement += 0.5 * trajectory_.acceleration * seconds_passed * seconds_passed;
  // v = a0 * dt
  trajectory_.velocity += trajectory_.acceleration * seconds_passed;
  return trajectory_;
}

const FakeTrajectory::Config &FakeTrajectory::getConfig() const
{
  return config_;
}

}  // namespace hyped::sensors
