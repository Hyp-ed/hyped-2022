#include "imu_manager.hpp"

#include <memory>

#include <sensors/fake_imu.hpp>
#include <sensors/imu.hpp>
#include <utils/config.hpp>
#include <utils/system.hpp>
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

ImuManager::ImuManager(utils::Logger &log,
                       std::array<std::unique_ptr<IImu>, data::Sensors::kNumImus> imus)
    : imus_(std::move(imus))
{
  log.INFO("IMU-MANAGER", "initialisation complete");
}

std::optional<std::unique_ptr<ImuManager>> ImuManager::fromFile(
  utils::Logger &log, const std::string &path, std::shared_ptr<FakeTrajectory> fake_trajectory)
{
  const auto fake_imus_optional = FakeImu::fromFile(log, path, fake_trajectory);
  if (!fake_imus_optional) {
    log.ERR("IMU-MANAGER", "failed to initialise fake imus");
    auto &system    = utils::System::getSystem();
    system.running_ = false;
    return std::nullopt;
  }
  std::array<std::unique_ptr<IImu>, data::Sensors::kNumImus> imus;
  for (size_t i = 0; i < data::Sensors::kNumImus; ++i) {
    imus.at(i) = std::move(std::make_unique<FakeImu>(fake_imus_optional->at(i)));
  }
  return std::make_unique<ImuManager>(ImuManager(log, std::move(imus)));
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
