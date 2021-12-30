#include "gpio_manager.hpp"

#include <utils/config.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

GpioManager::GpioManager(utils::Logger &log)
    : Thread(log),
      sys_(utils::System::getSystem()),
      data_(data::Data::getInstance())
{
  // clear HPSSRs if default is high
  for (size_t i = 0; i < data::Batteries::kNumHPBatteries; ++i) {
    high_power_ssr_.push_back(std::make_unique<utils::io::GPIO>(
      sys_.config->sensors.hp_shutoff.at(i), utils::io::gpio::kOut));
    high_power_ssr_.at(i)->clear();
    log_.INFO("BMS-MANAGER", "HP SSR %d has been initialised CLEAR", i);
  }
  // master switch to keep pod on
  master_ = std::make_unique<utils::io::GPIO>(sys_.config->sensors.master, utils::io::gpio::kOut);
  master_->set();
  log_.INFO("BMS-MANAGER", "Master switch SET");
  // add additional GPIO in format above
  Thread::yield();
}

void GpioManager::clearHighPower()
{
  master_->clear();  // important to clear this first
  for (size_t i = 0; i < data::Batteries::kNumHPBatteries; ++i) {
    high_power_ssr_[i]->clear();  // HP off until kReady State
  }
}

void GpioManager::setHighPower()
{
  for (auto &ssr : high_power_ssr_) {
    ssr->set();
    sleep(50);
  }
  master_->set();
}

void GpioManager::run()
{
  while (sys_.running_) {
    const auto state = data_.getStateMachineData().current_state;
    if (state != previous_state_) {
      switch (state) {
        case data::State::kIdle:
        case data::State::kAccelerating:
        case data::State::kCruising:
        case data::State::kCalibrating:
        case data::State::kNominalBraking:
          break;
        case data::State::kEmergencyBraking:
        case data::State::kFailureStopped:
          clearHighPower();
          log_.ERR("GPIO-MANAGER", "Emergency State! HP SSR cleared");
          break;
        case data::State::kFinished:
          clearHighPower();
          log_.INFO("GPIO-MANAGER", "kFinished reached...HP off");
          break;
        case data::State::kReady:
          setHighPower();
          log_.INFO("GPIO-MANAGER", "kReady...HP SSR set and HP on");
          break;
        case data::State::kInvalid:
          clearHighPower();  // shutting down HP asap
          log_.ERR("GPIO-MANAGER", "Unknown State! HP SSR cleared, shutting down!");

          // signalling failure to get out of undefied behaviour
          auto batteries_data          = data_.getBatteriesData();
          batteries_data.module_status = data::ModuleStatus::kCriticalFailure;
          data_.setBatteriesData(batteries_data);
          break;
      }
    }
    previous_state_ = state;
    sleep(100);
  }
}

}  // namespace hyped::sensors
