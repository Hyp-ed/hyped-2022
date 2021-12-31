#include "fake_trajectory.hpp"

#include <fstream>
#include <random>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

namespace hyped::sensors {

std::optional<FakeTrajectory::Config> FakeTrajectory::readConfig(utils::Logger &log,
                                                                 const std::string &path)
{
  std::ifstream input_stream(path);
  if (!input_stream.is_open()) {
    log.ERR("FAKE-TRAJECTORY", "Failed to open config file at %s", path.c_str());
    return std::nullopt;
  }
  rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
  rapidjson::Document document;
  document.ParseStream(input_stream_wrapper);
  if (document.HasParseError()) {
    log.ERR("FAKE-TRAJECTORY", "Failed to parse config file at %s", path.c_str());
    return std::nullopt;
  }
  FakeTrajectory::Config config;
  if (!document.HasMember("maximum_acceleration")) {
    log.ERR("FAKE-TRAJECTORY",
            "Missing required field 'maximum_acceleration' in configuration file at %s",
            path.c_str());
    return std::nullopt;
  }
  config.maximum_acceleration = document["maximum_acceleration"].GetDouble();
  if (!document.HasMember("braking_deceleration")) {
    log.ERR("FAKE-TRAJECTORY",
            "Missing required field 'braking_deceleration' in configuration file at %s",
            path.c_str());
    return std::nullopt;
  }
  config.braking_deceleration = document["braking_deceleration"].GetDouble();
  if (!document.HasMember("cruising_deceleration")) {
    log.ERR("FAKE-TRAJECTORY",
            "Missing required field 'cruising_deceleration' in configuration file at %s",
            path.c_str());
    return std::nullopt;
  }
  config.cruising_deceleration = document["cruising_deceleration"].GetDouble();
  return config;
}

std::optional<FakeTrajectory> FakeTrajectory::fromFile(utils::Logger &log, const std::string &path)
{
  const auto config_optional = readConfig(log, path);
  if (!config_optional) {
    log.ERR("FAKE-TRAJECTORY", "Failed constructing");
    return std::nullopt;
  }
  return FakeTrajectory(*config_optional);
}

FakeTrajectory::FakeTrajectory(const Config &config)
    : config_(config),
      data_(data::Data::getInstance()),
      last_update_(utils::Timer::getTimeMicros()),
      last_trajectory_({0.0, 0.0, 0.0})
{
}

FakeTrajectory::Trajectory FakeTrajectory::getTrajectory()
{
  const auto stm_data              = data_.getStateMachineData();
  const uint64_t now               = utils::Timer::getTimeMicros();
  const data::nav_t seconds_passed = static_cast<double>(last_update_ - now) / 1e6;
  last_update_                     = now;
  data::nav_t current_acceleration;
  switch (stm_data.current_state) {
    case data::State::kIdle:
    case data::State::kPreCalibrating:
    case data::State::kCalibrating:
    case data::State::kReady:
    case data::State::kFailureStopped:
    case data::State::kFinished:
    case data::State::kInvalid:
      last_trajectory_.acceleration = 0.0;
      last_trajectory_.velocity     = 0.0;
      return last_trajectory_;
    case data::State::kAccelerating:
      current_acceleration = config_.maximum_acceleration;
      break;
    case data::State::kCruising:
      current_acceleration = -config_.cruising_deceleration;
      break;
    case data::State::kNominalBraking:
    case data::State::kEmergencyBraking:
      current_acceleration = -config_.braking_deceleration;
      break;
  }
  // s = s0 + v0 * dt + 1/2 * a0 * dt^2
  last_trajectory_.displacement += last_trajectory_.velocity * seconds_passed;
  last_trajectory_.displacement += 0.5 * current_acceleration * seconds_passed * seconds_passed;
  // v = a0 * dt
  last_trajectory_.velocity += last_trajectory_.acceleration * seconds_passed;
  // a = a0
  last_trajectory_.acceleration = current_acceleration;
  return last_trajectory_;
}

const FakeTrajectory::Config &FakeTrajectory::getConfig() const
{
  return config_;
}

}  // namespace hyped::sensors
