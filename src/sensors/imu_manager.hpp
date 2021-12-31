#pragma once

#include "fake_trajectory.hpp"
#include "interface.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped::sensors {
/**
 * @brief creates class to hold multiple IMUs and respective data.
 *
 */
class ImuManager : public utils::concurrent::Thread {
  using DataArray = data::DataPoint<std::array<data::ImuData, data::Sensors::kNumImus>>;

 public:
  ImuManager(utils::Logger &log);

  ImuManager(utils::Logger &log, std::shared_ptr<FakeTrajectory> fake_trajectory);

  /**
   * @brief Calibrate IMUs then begin collecting data.
   */
  void run() override;

 private:
  std::array<std::unique_ptr<IImu>, data::Sensors::kNumImus> imus_;
};

}  // namespace hyped::sensors
