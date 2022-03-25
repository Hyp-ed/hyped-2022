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
  static std::optional<std::vector<FakeImu>> fromFile(
    const std::string &path, std::shared_ptr<FakeTrajectory> fake_trajectory);

 private:
  const Config config_;
  data::Data &data_;
  std::shared_ptr<FakeTrajectory> fake_trajectory_;
  bool is_operational_;

  FakeImu(const Config &config, std::shared_ptr<FakeTrajectory> fake_trajectory);
  static std::optional<std::vector<Config>> readConfigs(utils::Logger &log,
                                                        const std::string &path);

  data::NavigationVector getAccurateAcceleration();
  data::NavigationVector addNoiseToAcceleration(const data::NavigationVector acceleration) const;
};

}  // namespace hyped::sensors
