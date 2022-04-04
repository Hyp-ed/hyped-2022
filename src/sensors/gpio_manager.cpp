#include "gpio_manager.hpp"

#include <utils/timer.hpp>

namespace hyped::sensors {

GpioManager::GpioManager(utils::Logger log, const Config &config)
    : Thread(log),
      sys_(utils::System::getSystem()),
      data_(data::Data::getInstance()),
      config_(config)
{
  // clear HPSSRs if default is high
  for (size_t i = 0; i < data::FullBatteryData::kNumHPBatteries; ++i) {
    high_power_ssr_.push_back(std::make_unique<utils::io::Gpio>(config_.high_power_ssr_pins.at(i),
                                                                utils::io::Gpio::Direction::kOut));
    high_power_ssr_.at(i)->clear();
    log_.info("HP SSR %d has been initialised CLEAR", i);
  }
  // master switch to keep pod on
  master_ = std::make_unique<utils::io::Gpio>(config_.master_switch_pin,
                                              utils::io::Gpio::Direction::kOut);
  master_->set();
  log_.info("Master switch SET");
  // add additional GPIO in format above
  Thread::yield();
}

void GpioManager::clearHighPower()
{
  data::Sensors sensors_data_struct = data_.getSensorsData();
  master_->clear();  // important to clear this first
  for (size_t i = 0; i < data::FullBatteryData::kNumHPBatteries; ++i) {
    high_power_ssr_[i]->clear();  // HP off until kReady State
  }
  sensors_data_struct.high_power_off = true;  // all SSRs in HP off
  data_.setSensorsData(sensors_data_struct);
}

void GpioManager::setHighPower()
{
  for (auto &ssr : high_power_ssr_) {
    ssr->set();
    sleep(50);
  }
  master_->set();
  auto sensors_data_struct           = data_.getSensorsData();
  sensors_data_struct.high_power_off = false;  // all SSRs in HP on
  data_.setSensorsData(sensors_data_struct);
}

void GpioManager::run()
{
  while (sys_.isRunning()) {
    const auto state = data_.getStateMachineData().current_state;
    if (state != previous_state_) {
      switch (state) {
        case data::State::kIdle:
        case data::State::kPreCalibrating:
        case data::State::kAccelerating:
        case data::State::kCruising:
        case data::State::kCalibrating:
        case data::State::kPreBraking:
          clearHighPower();
          log_.error("Braking! HP SSR cleared");
          break;
        case data::State::kNominalBraking:
        case data::State::kReady:
          break;
        case data::State::kFailurePreBraking:
          clearHighPower();
          log_.error("Failure Braking! HP SSR cleared");
          break;
        case data::State::kFailureBraking:
        case data::State::kFailureStopped:
          clearHighPower();
          log_.error("Emergency State! HP SSR cleared");
          break;
        case data::State::kFinished:
          clearHighPower();
          log_.info("kFinished reached...HP off");
          break;
        case data::State::kPreReady:
          setHighPower();
          log_.info("kPreReady...HP SSR set and HP on");
          break;
        case data::State::kInvalid:
          clearHighPower();  // shutting down HP asap
          log_.error("Unknown State! HP SSR cleared, shutting down!");

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
