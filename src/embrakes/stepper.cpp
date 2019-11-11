/*
* Author: Kornelija Sukyte
* Organisation: HYPED
* Date:
* Description:
*
*    Copyright 2019 HYPED
*    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
*    except in compliance with the License. You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
*    Unless required by applicable law or agreed to in writing, software distributed under
*    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
*    either express or implied. See the License for the specific language governing permissions and
*    limitations under the License.
*/

#include "embrakes/stepper.hpp"

namespace hyped {
namespace embrakes {

Stepper::Stepper(uint8_t enable_pin, uint8_t button_pin, Logger& log, uint8_t id)
    : log_(log),
      data_(data::Data::getInstance()),
      brake_id_(id),
      em_brakes_data_(data_.getEmergencyBrakesData()),
      command_pin_(enable_pin, utils::io::gpio::kOut, log_),
      button_(button_pin, utils::io::gpio::kIn, log_),
      is_clamped_(true)
{
  GPIO command_pin_(enable_pin, utils::io::gpio::kOut, log_);
  GPIO button_(button_pin, utils::io::gpio::kIn, log_);
}

void Stepper::checkHome()
{
  if (button_.read() && !em_brakes_data_.brakes_retracted[brake_id_-1]) {
    em_brakes_data_.brakes_retracted[brake_id_-1] = true;
    data_.setEmergencyBrakesData(em_brakes_data_);
  } else if (!button_.read() && em_brakes_data_.brakes_retracted[brake_id_-1]) {
    em_brakes_data_.brakes_retracted[brake_id_-1] = false;
    data_.setEmergencyBrakesData(em_brakes_data_);
  }
}

void Stepper::sendRetract()
{
  log_.INFO("Brakes", "Sending a retract message to brake %i", brake_id_);
  command_pin_.clear();
  is_clamped_ = false;
}

void Stepper::sendClamp()
{
  log_.INFO("Brakes", "Sending a retract message to brake %i", brake_id_);
  command_pin_.set();
  is_clamped_ = true;
}

void Stepper::checkAccFailure()
{
  if (!button_.read()) {
    timer = utils::Timer::getTimeMicros();
    if ((utils::Timer::getTimeMicros() - timer > 200000) && !button_.read()) {
      log_.ERR("Brakes", "Brake %b failure", brake_id_);
      em_brakes_data_.module_status = ModuleStatus::kCriticalFailure;
      data_.setEmergencyBrakesData(em_brakes_data_);
    } else {
      return;
    }
  }
}

void Stepper::checkBrakingFailure()
{
  if (button_.read()) {
    timer = utils::Timer::getTimeMicros();
    if ((utils::Timer::getTimeMicros() - timer > 200000) && button_.read()) {
      log_.ERR("Brakes", "Brake %b failure", brake_id_);
      em_brakes_data_.module_status = ModuleStatus::kCriticalFailure;
      data_.setEmergencyBrakesData(em_brakes_data_);
    } else {
      return;
    }
  }
}

bool Stepper::checkClamped()
{
  return is_clamped_;
}

}}  // namespace hyped::embrakes
