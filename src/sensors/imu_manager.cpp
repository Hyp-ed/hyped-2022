#include "imu_manager.hpp"

#include <sensors/fake_imu.hpp>
#include <sensors/imu.hpp>
#include <utils/config.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

ImuManager::ImuManager(utils::Logger &log) : Thread(log)
{
  utils::io::SPI::getInstance().setClock(utils::io::SPI::Clock::k4MHz);

  auto &sys = utils::System::getSystem();
  for (size_t i = 0; i < data::Sensors::kNumImus; i++) {
    imus_[i] = std::make_unique<Imu>(log, sys.config->sensors.chip_select[i], false);
  }
  log_.INFO("IMU-MANAGER", "initialisation complete");
}

ImuManager::ImuManager(utils::Logger &log, std::shared_ptr<FakeTrajectory> fake_trajectory)
    : Thread(log)
{
  // TODO(miltfra): Test noise value
  constexpr data::nav_t kNoise = 0.2;
  for (size_t i = 0; i < data::Sensors::kNumImus; i++) {
    imus_[i] = std::make_unique<FakeImu>(fake_trajectory, kNoise);
  }
  log_.INFO("IMU-MANAGER", "initialisation complete");
}

void ImuManager::run()
{
  auto &sys  = utils::System::getSystem();
  auto &data = data::Data::getInstance();
  while (sys.running_) {
    DataArray imu_data;
    for (size_t i = 0; i < imus_.size(); ++i) {
      imu_data.value[i] = imus_.at(i)->getData();
    }
    imu_data.timestamp = utils::Timer::getTimeMicros();
    data.setSensorsImuData(imu_data);
  }
}
}  // namespace hyped::sensors
