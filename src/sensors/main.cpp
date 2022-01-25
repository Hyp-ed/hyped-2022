#include "main.hpp"

#include <sensors/fake_keyence.hpp>
#include <sensors/fake_temperature.hpp>
#include <sensors/gpio_counter.hpp>
#include <sensors/temperature.hpp>

namespace hyped::sensors {

Main::Main()
    : utils::concurrent::Thread(
      utils::Logger("SENSORS", utils::System::getSystem().config_.log_level_sensors)),
      sys_(utils::System::getSystem()),
      data_(data::Data::getInstance())
{
  battery_manager_ = BmsManager::fromFile(sys_.config_.bms_config_path);
  if (!battery_manager_) {
    log_.error("failed to initialise bms");
    sys_.stop();
    return;
  }
  if (sys_.config_.use_fake_trajectory) {
    const auto fake_trajectory_optional
      = FakeTrajectory::fromFile(log_, sys_.config_.fake_trajectory_config_path);
    if (!fake_trajectory_optional) {
      log_.error("failed to initialise fake trajectory");
      sys_.stop();
      return;
    }
    const auto fake_trajectory = std::make_shared<FakeTrajectory>(*fake_trajectory_optional);
    const auto fake_keyences_optional
      = FakeKeyence::fromFile(log_, sys_.config_.keyence_config_path, fake_trajectory);
    if (!fake_keyences_optional) {
      log_.error("failed to initialise fake keyence");
      sys_.stop();
      return;
    }
    for (size_t i = 0; i < data::Sensors::kNumKeyence; ++i) {
      keyences_.at(i) = std::move(std::make_unique<FakeKeyence>(fake_keyences_optional->at(i)));
    }
    imu_manager_ = ImuManager::fromFile(sys_.config_.imu_config_path, fake_trajectory);
    if (!imu_manager_) {
      log_.error("failed to initialise fake imus");
      sys_.stop();
      return;
    }
  } else {
    // Real trajectory sensors
    auto keyence_pins = keyencePinsFromFile(sys_.config_.keyence_config_path);
    if (!keyence_pins) {
      log_.error("failed to initialise keyence");
      sys_.stop();
      return;
    }
    for (size_t i = 0; i < data::Sensors::kNumKeyence; ++i) {
      auto keyence = std::make_unique<GpioCounter>(keyence_pins->at(i));
      keyence->start();
      keyences_[i] = std::move(keyence);
    }
    auto imu_pins = imuPinsFromFile(sys_.config_.imu_config_path);
    if (!imu_pins) {
      log_.error("failed to initialise IMUs");
      sys_.stop();
      return;
    }
    imu_manager_ = std::make_unique<ImuManager>(
      utils::Logger("IMU-MANAGER", sys_.config_.log_level_sensors), *imu_pins);
    if (!imu_manager_) {
      log_.error("failed to initialise imus");
      sys_.stop();
      return;
    }
  }

  // Temperature
  if (sys_.config_.use_fake_temperature_fail) {
    temperature_ = std::make_unique<FakeTemperature>(log_, true);
  } else if (sys_.config_.use_fake_temperature) {
    temperature_ = std::make_unique<FakeTemperature>(log_, false);
  } else {
    auto temperature_pin = temperaturePinFromFile(sys_.config_.temperature_config_path);
    if (!temperature_pin) {
      log_.error("failed to initialise temperature sensor");
      sys_.stop();
      return;
    }
    temperature_ = std::make_unique<Temperature>(log_, *temperature_pin);
  }

  // kReady for state machine transition
  sensors_               = data_.getSensorsData();
  sensors_.module_status = data::ModuleStatus::kReady;
  data_.setSensorsData(sensors_);
  log_.info("Sensors have been initialised");
}

void Main::checkTemperature()
{
  temperature_->run();  // not a thread
  data_.setTemperature(temperature_->getData());
  if (data_.getTemperature() > 85 && !log_error_) {
    log_.info("PCB temperature is getting a wee high...sorry Cheng");
    log_error_ = true;
  }
}

void Main::run()
{
  battery_manager_->start();
  imu_manager_->start();

  auto current_keyence  = data_.getSensorsKeyenceData();
  auto previous_keyence = current_keyence;

  int temp_count = 0;
  while (sys_.isRunning()) {
    bool keyence_updated = false;
    for (size_t i = 0; i < current_keyence.size(); ++i) {
      if (current_keyence.at(i).timestamp > previous_keyence.at(i).timestamp) {
        keyence_updated = true;
        break;
      }
    }
    if (keyence_updated) {
      data_.setSensorsKeyenceData(current_keyence);
      previous_keyence = current_keyence;
    }
    for (size_t i = 0; i < data::Sensors::kNumKeyence; ++i) {
      current_keyence.at(i) = keyences_[i]->getData();
    }
    Thread::sleep(10);
    temp_count++;
    // only check every 20 cycles
    if (temp_count % 20 == 0) {
      checkTemperature();
      // avoid overflow
      temp_count = 0;
    }
  }
  imu_manager_->join();
  battery_manager_->join();
}
}  // namespace hyped::sensors
