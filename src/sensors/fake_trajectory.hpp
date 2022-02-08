#pragma once

#include <optional>
#include <string>

#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

class FakeTrajectory {
 public:
  struct Config {
    data::nav_t maximum_acceleration;
    data::nav_t braking_deceleration;
    data::nav_t cruising_deceleration;
  };
  struct Trajectory {
    data::nav_t acceleration;
    data::nav_t velocity;
    data::nav_t displacement;
  };
  Trajectory getTrajectory();
  const Config &getConfig() const;
  static std::optional<FakeTrajectory> fromFile(const std::string &path);

 private:
  const Config config_;
  data::Data &data_;
  uint64_t last_update_;
  Trajectory trajectory_;

  FakeTrajectory(const Config &config);
  static std::optional<Config> readConfig(utils::Logger &log, const std::string &path);
};

}  // namespace hyped::sensors
