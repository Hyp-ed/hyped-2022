#pragma once

#include "fake_trajectory.hpp"
#include "interface.hpp"

#include <memory>
#include <optional>

namespace hyped::sensors {

class FakeKeyence : public ICounter {
 public:
  struct Config {
    std::optional<data::State> failure_in_state;
    data::nav_t noise;
  };
  FakeKeyence(std::shared_ptr<FakeTrajectory> fake_trajectory, const Config config);

  data::CounterData getData() override;

  bool isOnline() override { return true; }

 private:
  const Config config_;
  data::Data &data_;
  std::shared_ptr<FakeTrajectory> fake_trajectory_;

  data::CounterData previous_data_;
  data::nav_t addNoiseToDisplacement(const data::nav_t displacement) const;
};

}  // namespace hyped::sensors
