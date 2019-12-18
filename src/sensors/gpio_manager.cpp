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
    : sys_(utils::System::getSystem()),
      data_(Data::getInstance())
{
  if (!sys_.battery_test) {
    // Set SSR switches for real system

    // IMD ssr
    imd_out_ = new GPIO(sys_.config->sensors.IMDOut, utils::io::gpio::kOut);
    imd_out_->set();
    log_.INFO("BMS-MANAGER", "IMD has been initialised SET");

    // clear HPSSRs if default is high
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      hp_ssr_[i] = new GPIO(sys_.config->sensors.HPSSR[i], utils::io::gpio::kOut);
      hp_ssr_[i]->clear();      // HP off until kReady State
      log_.INFO("BMS-MANAGER", "HP SSR %d has been initialised CLEAR", i);
    }
    hp_master_ = new GPIO(sys_.config->sensors.hp_master, utils::io::gpio::kOut);
    hp_master_->clear();
    log_.INFO("BMS-MANAGER", "HP SSRs has been initialised CLEAR");

    // Set embrakes ssr
    embrakes_ssr_ = new GPIO(sys_.config->sensors.embrakes, utils::io::gpio::kOut);
    embrakes_ssr_->set();
    log_.INFO("BMS-MANAGER", "Embrake SSR has been set");
  }
}

void GpioManager::clearHP()
{
  if (!sys_.battery_test) {
    if (!(sys_.fake_batteries || sys_.fake_batteries_fail)) {
      hp_master_->clear();  // important to clear this first
      for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
        hp_ssr_[i]->clear();      // HP off until kReady State
      }
    }
  }
}

void GpioManager::setHP()
{
  if (!sys_.battery_test) {
    if (!(sys_.fake_batteries || sys_.fake_batteries_fail)) {
      for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
        hp_ssr_[i]->set();
        sleep(50);
      }
      hp_master_->set();
    }
  }
}

void GpioManager::run()
{
  data::State state = data_.getStateMachineData().current_state;
  if (state == data::State::kEmergencyBraking || state == data::State::kFailureStopped) {
    clearHP();
    embrakes_ssr_->clear();     // actuate brakes in emergency state
    if (state != previous_state_)
      log_.ERR("BMS-MANAGER", "Emergency State! HP SSR cleared and Embrakes actuated");
  } else if (state == data::State::kFinished) {
    clearHP();
    if (state != previous_state_)
      log_.INFO("BMS-MANAGER", "kFinished reached...HP off");
  } else if (state == data::State::kReady) {
    setHP();
    if (state != previous_state_)
      log_.INFO("BMS-MANAGER", "kReady...HP SSR set and HP on");
  }
  previous_state_ = state;
  sleep(100);

}

}}  // namespace hyped::sensors
