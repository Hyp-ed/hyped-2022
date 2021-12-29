#pragma once

#include <optional>
#include <string>

#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

class FakeTrajectory {
 public:
  struct Trajectory {
    data::nav_t acceleration;
    data::nav_t velocity;
    data::nav_t displacement;
  };
  FakeTrajectory::Trajectory getTrajectory();
  static std::optional<FakeTrajectory> fromFile(utils::Logger &log, const std::string &path);

 private:
  struct Config {
    data::nav_t maximum_acceleration;
    data::nav_t braking_deceleration;
    data::nav_t cruising_deceleration;
  };
  const Config config_;
  data::Data &data_;
  uint64_t last_update_;
  Trajectory last_trajectory_;

  FakeTrajectory(const Config &config);
  static std::optional<Config> readConfig(utils::Logger &log, const std::string &path);
};

}  // namespace hyped::sensors
