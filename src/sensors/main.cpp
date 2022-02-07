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

Main::Main(uint8_t id, utils::Logger &log)
    : Thread(id, log),
      data_(data::Data::getInstance()),
      sys_(utils::System::getSystem()),
      log_(log),
      pins_{static_cast<uint8_t>(sys_.config->sensors.keyence_l),
            static_cast<uint8_t>(sys_.config->sensors.keyence_r)},  // NOLINT
      imu_manager_(new ImuManager(log)),
      battery_manager_(new BmsManager(log))
{
  if (!(sys_.fake_keyence || sys_.fake_keyence_fail)) {
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      GpioCounter *keyence = new GpioCounter(log_, pins_[i]);
      keyence->start();
      keyences_[i] = keyence;
    }
  } else if (sys_.fake_keyence_fail) {
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      // miss four stripes in a row after 20th, 2000 micros during peak velocity
      keyences_[i] = new FakeGpioCounter(log_, true, "data/in/gpio_counter_fail_run.txt");
    }
  } else {
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      keyences_[i] = new FakeGpioCounter(log_, false, "data/in/gpio_counter_normal_run.txt");
    }
  }
  if (!(sys_.fake_temperature || sys_.fake_temperature_fail)) {
    temperature_ = new Temperature(log_, sys_.config->sensors.thermistor);
  } else if (sys_.fake_temperature_fail) {
    // fake temperature fail case
    temperature_ = new FakeTemperature(log_, true);
  } else {
    temperature_ = new FakeTemperature(log_, false);
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

  converted_temp_ = temperature_->getData();
  if (converted_temp_ > 85 && !log_error_) {
    log_.INFO("Sensors", "PCB temperature is getting a wee high...sorry Cheng");
    log_error_ = true;
  }
}

void Main::checkPressure()
{
  pressure_->run();  // not a thread

  converted_pressure_ = pressure_->getData();
  if (converted_pressure_ > 1200 && !log_error_) {
    log_.INFO("Sensors", "PCB pressure is above what can be sensed");
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

  // Intialise temperature and pressure
  temp_ = data_.getSensorsData().temperature;
  pres_ = data_.getSensorsData().pressure;

  std::size_t iteration_count = 0;
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
    ++iteration_count;
    if (iteration_count % 20 == 0) {  // check every 20 cycles of main
      checkTemperature();
      checkPressure();
      // So that temp_count does not get huge
      iteration_count = 0;
    }
  }

  imu_manager_->join();
  battery_manager_->join();
}
}  // namespace sensors
}  // namespace hyped
