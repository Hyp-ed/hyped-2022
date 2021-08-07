#include "imu_manager.hpp"

#include <sensors/fake_imu.hpp>
#include <sensors/imu.hpp>
#include <utils/config.hpp>
#include <utils/timer.hpp>

namespace hyped {

using data::Data;
using data::Sensors;
using utils::System;

namespace sensors {
ImuManager::ImuManager(Logger &log)
    : Thread(log),
      sys_(System::getSystem()),
      data_(Data::getInstance()),
      imu_{0}
{
  if (!(sys_.fake_imu || sys_.fake_imu_fail)) {
    utils::io::SPI::getInstance().setClock(utils::io::SPI::Clock::k4MHz);

    for (int i = 0; i < data::Sensors::kNumImus; i++) {  // creates new real IMU objects
      imu_[i] = new Imu(log, sys_.config->sensors.chip_select[i], false);
    }
  } else if (sys_.fake_imu_fail) {
    for (int i = 0; i < data::Sensors::kNumImus; i++) {
      // change params to fail in kAcccelerating or kNominalBraking states
      imu_[i] = new FakeImuFromFile(log, "data/in/acc_state.txt", "data/in/decel_state.txt",
                                    "data/in/decel_state.txt", (i % 2 == 0), false);
    }
  } else {
    for (int i = 0; i < data::Sensors::kNumImus; i++) {
      imu_[i] = new FakeImuFromFile(log, "data/in/acc_state.txt", "data/in/decel_state.txt",
                                    "data/in/decel_state.txt", false, false);
    }
  }
  log_.INFO("IMU-MANAGER", "imu manager has been initialised");
}

void ImuManager::run()
{
  // collect real data while system is running
  while (sys_.running_) {
    for (int i = 0; i < data::Sensors::kNumImus; i++) {
      if (imu_[i]) imu_[i]->getData(&(sensors_imu_.value[i]));
    }
    sensors_imu_.timestamp = utils::Timer::getTimeMicros();
    data_.setSensorsImuData(sensors_imu_);
  }
}
}  // namespace sensors
}  // namespace hyped
