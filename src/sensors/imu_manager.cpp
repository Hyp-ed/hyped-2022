#include "imu_manager.hpp"

#include <memory>

#include <sensors/fake_imu.hpp>
#include <sensors/imu.hpp>
#include <utils/system.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

ImuManager::ImuManager(const std::array<uint32_t, data::Sensors::kNumImus> &imu_pins)
    : Thread(utils::Logger("IMU-MANAGER", utils::System::getSystem().config_.log_level_sensors))
{
  utils::io::SPI::getInstance().setClock(utils::io::SPI::Clock::k4MHz);
  for (size_t i = 0; i < data::Sensors::kNumImus; i++) {
    imus_.at(i) = std::make_unique<Imu>(imu_pins.at(i), false);
  }
}

ImuManager::ImuManager(std::array<std::unique_ptr<IImu>, data::Sensors::kNumImus> imus)
    : Thread(utils::Logger("IMU-MANAGER", utils::System::getSystem().config_.log_level_sensors)),
      imus_(std::move(imus))
{
}

std::unique_ptr<ImuManager> ImuManager::fromFile(const std::string &path,
                                                 std::shared_ptr<FakeTrajectory> fake_trajectory)
{
  auto &system = utils::System::getSystem();
  utils::Logger log("IMU-MANAGER", system.config_.log_level_sensors);
  const auto fake_imus_optional = FakeImu::fromFile(path, fake_trajectory);
  if (!fake_imus_optional) {
    log.error("failed to initialise fake imus");
    system.stop();
    return nullptr;
  }
  if (fake_imus_optional->size() != data::Sensors::kNumImus) {
    log.error("found %d fake imus but %d were expected", fake_imus_optional->size(),
              data::Sensors::kNumImus);
    system.stop();
    return nullptr;
  }
  std::array<std::unique_ptr<IImu>, data::Sensors::kNumImus> imus;
  for (size_t i = 0; i < data::Sensors::kNumImus; ++i) {
    imus.at(i) = std::make_unique<FakeImu>(fake_imus_optional->at(i));
  }
  return std::make_unique<ImuManager>(std::move(imus));
}

void ImuManager::run()
{
  log_.info("started");
  auto &sys  = utils::System::getSystem();
  auto &data = data::Data::getInstance();
  while (sys.isRunning()) {
    data::DataPoint<std::array<data::ImuData, data::Sensors::kNumImus>> imu_data;
    for (size_t i = 0; i < imus_.size(); ++i) {
      imu_data.value[i] = imus_.at(i)->getData();
    }
    imu_data.timestamp = utils::Timer::getTimeMicros();
    data.setSensorsImuData(imu_data);
  }
  log_.info("stopped");
}

}  // namespace hyped::sensors
