#pragma once

#include "fake_trajectory.hpp"
#include "interface.hpp"

#include <array>
#include <memory>
#include <optional>

#include <utils/logger.hpp>

namespace hyped::sensors {

class FakeKeyence : public ICounter {
 public:
  struct Config {
    std::optional<data::State> failure_in_state;
    data::nav_t noise;
  };
  data::CounterData getData() override;
  bool isOnline() override { return true; }
  const Config &getConfig() const;
  static std::optional<std::array<FakeKeyence, data::Sensors::kNumKeyence>> fromFile(
    utils::Logger &log, const std::string &path, std::shared_ptr<FakeTrajectory> fake_trajectory);

 private:
  const Config config_;
  data::Data &data_;
  std::shared_ptr<FakeTrajectory> fake_trajectory_;

  data::CounterData internal_data_;

  FakeKeyence(const Config &config, std::shared_ptr<FakeTrajectory> fake_trajectory);
  static std::optional<std::array<Config, data::Sensors::kNumKeyence>> readConfigs(
    utils::Logger &log, const std::string &path);
  data::nav_t addNoiseToDisplacement(const data::nav_t displacement) const;
};

}  // namespace hyped::sensors
