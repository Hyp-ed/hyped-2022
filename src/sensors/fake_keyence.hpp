#pragma once

#include "fake_trajectory.hpp"
#include "interface.hpp"

#include <memory>

namespace hyped::sensors {

class FakeKeyence : public ICounter {
 public:
  FakeKeyence(std::shared_ptr<FakeTrajectory> fake_trajectory, const data::nav_t noise);

  data::CounterData getData() override;

  bool isOnline() override { return true; }

 private:
  data::Data &data_;
  std::shared_ptr<FakeTrajectory> fake_trajectory_;
  const data::nav_t noise_;

  data::CounterData previous_data_;
  data::nav_t addNoiseToDisplacement(const data::nav_t displacement) const;
};

}  // namespace hyped::sensors
