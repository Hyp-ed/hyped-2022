/*
 * Author: Atte Niemi
 * Organisation: HYPED
 * Date:
 * Description:
 *
 *    Copyright 2020 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain a
 * copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include "embrakes/fake_stepper.hpp"

namespace hyped {
namespace embrakes {

FakeStepper::FakeStepper(Logger& log, uint8_t id)
    : log_(log),
      data_(data::Data::getInstance()),
      em_brakes_data_(data_.getEmergencyBrakesData()),
      brake_id_(id),
      is_clamped_(true),
      fake_button_(true) {}

void FakeStepper::checkHome()
{
  if (fake_button_ && !em_brakes_data_.brakes_retracted[brake_id_-1]) {
    em_brakes_data_.brakes_retracted[brake_id_-1] = true;
    data_.setEmergencyBrakesData(em_brakes_data_);
  } else if (!fake_button_ && em_brakes_data_.brakes_retracted[brake_id_-1]) {
    em_brakes_data_.brakes_retracted[brake_id_-1] = false;
    data_.setEmergencyBrakesData(em_brakes_data_);
  }
}

void FakeStepper::sendRetract()
{
  log_.INFO("Fake Stepper", "Sending a retract message to brake %i", brake_id_);
  fake_button_ = true;
  is_clamped_ = false;
}

void FakeStepper::sendClamp()
{
  log_.INFO("Fake Stepper", "Sending a retract message to brake %i", brake_id_);
  fake_button_ = false;
  is_clamped_ = true;
}

void FakeStepper::checkAccFailure()
{
  if (!fake_button_) {
    log_.ERR("Brakes", "Brake %b failure", brake_id_);
    em_brakes_data_.module_status = ModuleStatus::kCriticalFailure;
    data_.setEmergencyBrakesData(em_brakes_data_);
  }
  return;
}

void FakeStepper::checkBrakingFailure()
{
  if (fake_button_) {
    log_.ERR("Brakes", "Brake %b failure", brake_id_);
    em_brakes_data_.module_status = ModuleStatus::kCriticalFailure;
    data_.setEmergencyBrakesData(em_brakes_data_);
  }
  return;
}

bool FakeStepper::checkClamped() { return is_clamped_; }
}  // namespace embrakes
}  // namespace hyped
