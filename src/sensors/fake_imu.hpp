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
  FakeImu(std::shared_ptr<FakeTrajectory> fake_trajectory, const data::nav_t noise);

  bool isOnline() override { return true; }

  /*
   * @brief     A function that gets the imu data at the time of call. The function will return
   *            the same data point if the time period since the last update isn't long enough. It
   *            will also skip a couple of data points if the time since the last call has been
   *            sufficiently long.
   */
  data::ImuData getData() override;

 private:
  data::Data &data_;
  std::shared_ptr<FakeTrajectory> fake_trajectory_;
  const data::nav_t noise_;

  /**
   * @return NavigationVector zero acceleration as a vector
   */
  data::NavigationVector getZeroAcc() const;

  data::NavigationVector getAccurateAcceleration();
  data::NavigationVector addNoiseToAcceleration(const data::NavigationVector acceleration) const;
};

}  // namespace hyped::sensors
