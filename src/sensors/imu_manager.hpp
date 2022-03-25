#pragma once

#include "fake_trajectory.hpp"
#include "imu.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped::sensors {

class ImuManager : public utils::concurrent::Thread {
 public:
  static std::unique_ptr<ImuManager> fromFile(const std::string &path,
                                              std::shared_ptr<FakeTrajectory> fake_trajectory);
  ImuManager(const std::array<uint32_t, data::Sensors::kNumImus> &imu_pins);
  ImuManager(std::array<std::unique_ptr<IImu>, data::Sensors::kNumImus> imus);

  /**
   * @brief Calibrate IMUs then begin collecting data.
   */
  void run() override;

 private:
  std::array<std::unique_ptr<IImu>, data::Sensors::kNumImus> imus_;
};

}  // namespace hyped::sensors
