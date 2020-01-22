/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description:
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */


#include "sensors/gpio_manager.hpp"

#include "utils/timer.hpp"
#include "utils/config.hpp"

namespace hyped {
namespace sensors {

GpioManager::GpioManager(Logger& log)
    : Thread(log),
      sys_(utils::System::getSystem()),
      data_(Data::getInstance())
{
    // clear HPSSRs if default is high
  for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
    hp_ssr_.push_back(new GPIO(sys_.config->sensors.hp_shutoff[i], utils::io::gpio::kOut));
    hp_ssr_[i]->clear();
    log_.INFO("BMS-MANAGER", "HP SSR %d has been initialised CLEAR", i);
  }
  // master switch to keep pod on
  master_ = new GPIO(sys_.config->sensors.master, utils::io::gpio::kOut);
  master_->set();
  log_.INFO("BMS-MANAGER", "Master switch SET");

  // add additional GPIO in format above
  Thread::yield();
}

void GpioManager::clearHP()
{
  master_->clear();  // important to clear this first
  for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
    hp_ssr_[i]->clear();      // HP off until kReady State
  }
}

void GpioManager::setHP()
{
  for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
    hp_ssr_[i]->set();
    sleep(50);
  }
  master_->set();
}

void GpioManager::run()
{
  while (sys_.running_) {
    /**
     * Add module status for your appropriate module and have a switch statement to check each
     * appropriate that needs actuation of a hardware device. For example, we need to turn on
     * high power right before launch in kReady state and shut off high power during a failure.
     * See examples below. Use previous_<module>_status_ and previous_state_ to prevent repetitive
     * actuation.
     */

    data::ModuleStatus battery_status = data_.getBatteriesData().module_status;
    data::State state = data_.getStateMachineData().current_state;

    // kStart and kInit default clear from constructor
    switch (battery_status) {
      case data::ModuleStatus::kCriticalFailure:
        if (battery_status != previous_battery_status_) {
          clearHP();
          log_.ERR("GPIO-MANAGER", "Battery Failure! HP SSR cleared");
        }
      case data::ModuleStatus::kReady:
        if (battery_status != previous_battery_status_) {
          setHP();
          log_.ERR("GPIO-MANAGER", "Module Status kReady! HP SSR set");
        }
    }

    switch (state) {
      case data::State::kEmergencyBraking:
        if (state != previous_state_) {
          clearHP();
          log_.ERR("GPIO-MANAGER", "Emergency State! HP SSR cleared");
        }
      case data::State::kFailureStopped:
        if (state != previous_state_) {
          clearHP();
          log_.ERR("GPIO-MANAGER", "Emergency State! HP SSR cleared");
        }
      case data::State::kFinished:
        if (state != previous_state_) {
          clearHP();
          log_.INFO("GPIO-MANAGER", "kFinished reached...HP off");
        }
      case data::State::kReady:
        if (state != previous_state_) {
          setHP();
          log_.INFO("GPIO-MANAGER", "kReady...HP SSR set and HP on");
        }
    }
    previous_battery_status_ = battery_status;
    previous_state_ = state;
    sleep(100);
  }
}

}}  // namespace hyped::sensors
