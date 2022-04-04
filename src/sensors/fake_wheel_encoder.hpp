#pragma once

#include "fake_trajectory.hpp"
#include "sensor.hpp"

#include <array>
#include <memory>
#include <optional>

namespace hyped::sensors {

class FakeWheelEncoder : public ICounter {
 public:
  struct Config {
    std::optional<data::State> failure_in_state;
    data::nav_t noise;
  };
  data::CounterData getData() override;
  bool isOnline() override { return true; }
  const Config &getConfig() const;
  static std::optional<std::array<FakeWheelEncoder, data::Sensors::kNumEncoders>> fromFile(
    const std::string &path, std::shared_ptr<FakeTrajectory> fake_trajectory);

 private:
  const Config config_;
  data::Data &data_;
  std::shared_ptr<FakeTrajectory> fake_trajectory_;
  data::CounterData internal_data_;

  FakeWheelEncoder(const Config &config, std::shared_ptr<FakeTrajectory> fake_trajectory);
  static std::optional<std::array<Config, data::Sensors::kNumEncoders>> readConfigs(
    utils::Logger &log, const std::string &path);

  data::nav_t addNoiseToDisplacement(const data::nav_t displacement) const;
};

}  // namespace hyped::sensors
