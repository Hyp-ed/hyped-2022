#include "imu_manager.hpp"

#include <memory>

#include <sensors/fake_imu.hpp>
#include <sensors/imu.hpp>
#include <utils/system.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

ImuManager::ImuManager(utils::Logger log,
                       const std::array<uint32_t, data::Sensors::kNumImus> &imu_pins)
    : Thread(log)
{
  utils::io::SPI::getInstance().setClock(utils::io::SPI::Clock::k4MHz);
  for (size_t i = 0; i < data::Sensors::kNumImus; i++) {
    imus_.at(i) = std::make_unique<Imu>(log, imu_pins.at(i), false);
  }
}

ImuManager::ImuManager(utils::Logger log,
                       std::array<std::unique_ptr<IImu>, data::Sensors::kNumImus> imus)
    : Thread(log),
      imus_(std::move(imus))
{
}

std::unique_ptr<ImuManager> ImuManager::fromFile(const std::string &path,
                                                 std::shared_ptr<FakeTrajectory> fake_trajectory)
{
  auto &system = utils::System::getSystem();
  utils::Logger log("IMU-MANAGER", system.config_.log_level_sensors);
  const auto fake_imus_optional = FakeImu::fromFile(log, path, fake_trajectory);
  if (!fake_imus_optional) {
    log.error("failed to initialise fake imus");
    system.stop();
    return nullptr;
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
  while (sys.isRunning()) {
    data::DataPoint<std::array<data::ImuData, data::Sensors::kNumImus>> imu_data;
    for (size_t i = 0; i < imus_.size(); ++i) {
      imu_data.value[i] = imus_.at(i)->getData();
    }
    imu_data.timestamp = utils::Timer::getTimeMicros();
    data.setSensorsImuData(imu_data);
  }
}

}  // namespace hyped::sensors
