#pragma once

#include "interface.hpp"

#include <cstdint>

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

namespace hyped {

using utils::Logger;
using utils::concurrent::Thread;

namespace sensors {
/**
 * @brief creates class to hold multiple IMUs and respective data.
 *
 */
class ImuManager : public Thread {
  typedef data::DataPoint<std::array<ImuData, data::Sensors::kNumImus>> DataArray;

 public:
  /**
   * @brief Construct a new Imu Manager object
   *
   * @param log
   */
  explicit ImuManager(Logger &log);

  /**
   * @brief Calibrate IMUs then begin collecting data.
   */
  void run() override;

 private:
  utils::System &sys_;

  /**
   * @brief DataPoint array for all kNumImus
   */
  DataArray sensors_imu_;

  /**
   * @brief needs to be references because run() passes directly to data struct
   */
  data::Data &data_;

  ImuInterface *imu_[data::Sensors::kNumImus];
};

}  // namespace sensors
}  // namespace hyped
