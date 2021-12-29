#include "main.hpp"

#include <sensors/fake_gpio_counter.hpp>
#include <sensors/fake_temperature.hpp>
#include <sensors/gpio_counter.hpp>
#include <sensors/temperature.hpp>
#include <utils/config.hpp>

namespace hyped {

using data::Data;
using data::Sensors;
using data::StripeCounter;
using hyped::utils::concurrent::Thread;
using utils::System;

namespace sensors {

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
      for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
        // miss four stripes in a row after 20th, 2000 micros during peak velocity
        keyences_[i]
          = std::make_unique<FakeGpioCounter>(log_, true, "data/in/gpio_counter_fail_run.txt");
      }
    } else {
      for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
        keyences_[i]
          = std::make_unique<FakeGpioCounter>(log_, false, "data/in/gpio_counter_normal_run.txt");
      }
    }
    if (sys_.fake_imu_fail) {
      // TODO(miltfra): Implement failure mode for fake IMU
    } else {
      imu_manager_ = std::make_unique<ImuManager>(log, fake_trajectory);
    }
  } else {
    // Real trajectory sensors
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
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

bool Main::keyencesUpdated()
{
  for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
    if (prev_keyence_stripe_count_arr_[i].count.value != keyence_stripe_counter_arr_[i].count.value)
      return true;
  }
  return false;
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
  // start all managers
  battery_manager_->start();
  imu_manager_->start();

  // Initalise the keyence arrays
  keyence_stripe_counter_arr_    = data_.getSensorsData().keyence_stripe_counter;
  prev_keyence_stripe_count_arr_ = keyence_stripe_counter_arr_;

  int temp_count = 0;
  while (sys_.running_) {
    // We need to read the gpio counters and write to the data structure
    // If previous is not equal to the new data then update
    if (keyencesUpdated()) {
      // Update data structure, make prev reading same as this reading
      data_.setSensorsKeyenceData(keyence_stripe_counter_arr_);
      prev_keyence_stripe_count_arr_ = keyence_stripe_counter_arr_;
    }
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      keyences_[i]->getData(&keyence_stripe_counter_arr_[i]);
    }
    Thread::sleep(10);  // Sleep for 10ms
    temp_count++;
    if (temp_count % 20 == 0) {  // check every 20 cycles of main
      checkTemperature();
      // So that temp_count does not get huge
      temp_count = 0;
    }
  }

  imu_manager_->join();
  battery_manager_->join();
}
}  // namespace sensors
}  // namespace hyped
