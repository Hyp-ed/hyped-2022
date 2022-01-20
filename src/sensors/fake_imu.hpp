#pragma once

#include "fake_trajectory.hpp"
#include "interface.hpp"

#include <memory>
#include <string>
#include <vector>

#include <utils/logger.hpp>

namespace hyped::sensors {

class FakeImu : public IImu {
 public:
  struct Config {
    std::optional<data::State> failure_in_state;
    data::nav_t noise;
  };
  data::ImuData getData() override;
  bool isOnline() override { return true; }
  const Config &getConfig() const;
  static std::optional<std::array<FakeImu, data::Sensors::kNumImus>> fromFile(
    utils::Logger &log, const std::string &path, std::shared_ptr<FakeTrajectory> fake_trajectory);

 private:
  const Config config_;
  data::Data &data_;
  std::shared_ptr<FakeTrajectory> fake_trajectory_;

  FakeImu(const Config &config, std::shared_ptr<FakeTrajectory> fake_trajectory);
  static std::optional<std::array<Config, data::Sensors::kNumImus>> readConfigs(
    utils::Logger &log, const std::string &path);

  data::NavigationVector getZeroAcc() const;
  data::NavigationVector getAccurateAcceleration();
  data::NavigationVector addNoiseToAcceleration(const data::NavigationVector acceleration) const;
};

}  // namespace hyped::sensors
