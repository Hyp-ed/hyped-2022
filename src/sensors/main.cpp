#include "main.hpp"

#include <sensors/fake_keyence.hpp>
#include <sensors/fake_temperature.hpp>
#include <sensors/gpio_counter.hpp>
#include <sensors/temperature.hpp>
#include <utils/config.hpp>

namespace hyped::sensors {

Main::Main(const uint8_t id, utils::Logger &log)
    : Thread(id, log),
      sys_(utils::System::getSystem()),
      data_(data::Data::getInstance()),
      log_(log),
      keyence_pins_{static_cast<uint8_t>(sys_.config->sensors.keyence_l),
                    static_cast<uint8_t>(sys_.config->sensors.keyence_r)}
{
  battery_manager_ = std::make_unique<BmsManager>(log);
  if (sys_.fake_trajectory) {
    auto fake_trajectory_optional
      = FakeTrajectory::fromFile(log, "configurations/fake_trajectory.json");
    if (!fake_trajectory_optional) {
      log.ERR("SENSORS", "failed to initialise fake trajectory");
      sys_.running_ = false;
      return;
    }
    auto fake_trajectory = std::make_shared<FakeTrajectory>(*fake_trajectory_optional);
    if (sys_.fake_keyence_fail) {
      // TODO(miltfra): Implement failure mode for fake keyence
    } else {
      for (size_t i = 0; i < data::Sensors::kNumKeyence; ++i) {
        const FakeKeyence::Config config = {std::nullopt, 0.1};
        keyences_[i]                     = std::make_unique<FakeKeyence>(fake_trajectory, config);
      }
    }
    if (sys_.fake_imu_fail) {
      // TODO(miltfra): Implement failure mode for fake IMU
    } else {
      imu_manager_ = std::make_unique<ImuManager>(log, fake_trajectory);
    }
  } else {
    // Real trajectory sensors
    for (size_t i = 0; i < data::Sensors::kNumKeyence; ++i) {
      auto keyence = std::make_unique<GpioCounter>(log_, keyence_pins_[i]);
      keyence->start();
      keyences_[i] = std::move(keyence);
    }
    imu_manager_ = std::make_unique<ImuManager>(log);
  }

  // Temperature
  if (sys_.fake_temperature_fail) {
    temperature_ = std::make_unique<FakeTemperature>(log_, true);
  } else if (sys_.fake_temperature) {
    temperature_ = std::make_unique<FakeTemperature>(log_, false);
  } else {
    temperature_ = std::make_unique<Temperature>(log_, sys_.config->sensors.thermistor);
  }

  // kReady for state machine transition
  sensors_               = data_.getSensorsData();
  sensors_.module_status = data::ModuleStatus::kReady;
  data_.setSensorsData(sensors_);
  log_.INFO("Sensors", "Sensors have been initialised");
}

void Main::checkTemperature()
{
  temperature_->run();  // not a thread
  data_.setTemperature(temperature_->getData());
  if (data_.getTemperature() > 85 && !log_error_) {
    log_.INFO("Sensors", "PCB temperature is getting a wee high...sorry Cheng");
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
  while (sys_.running_) {
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
