/*
* Author: Kornelija Sukyte, Atte Niemi
* Organisation: HYPED
* Date:
* Description: Entrypoint class to the embrake module, started in it's own thread.
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

#include "main.hpp"
#include "utils/config.hpp"

namespace hyped {
namespace embrakes {

Main::Main(uint8_t id, Logger &log)
  : Thread(id, log),
    log_(log),
    data_(data::Data::getInstance()),
    sys_(utils::System::getSystem())
{
  // parse GPIO pins from config.txt file
  for (int i = 0; i < 2; i++) {
    command_pins_[i] = sys_.config->embrakes.command[i];
    button_pins_[i] = sys_.config->embrakes.button[i];
  }
  if (sys_.fake_embrakes) {
    m_brake_ = new FakeStepper(log_, 1);
    f_brake_ = new FakeStepper(log_, 2);
  } else {
    m_brake_ = new Stepper(command_pins_[0], button_pins_[0], log_, 1);
    f_brake_ = new Stepper(command_pins_[1], button_pins_[1], log_, 2);
  }
}

void Main::run()
{
  log_.INFO("Brakes", "Thread started");

  System &sys = System::getSystem();

  while (sys.running_) {
    // Get the current state of embrakes, state machine and telemetry modules from data
    em_brakes_ = data_.getEmergencyBrakesData();
    sm_data_ = data_.getStateMachineData();
    tlm_data_ = data_.getTelemetryData();

    switch (sm_data_.current_state) {
      case data::State::kIdle:
        if (tlm_data_.nominal_braking_command) {
          if (!m_brake_->checkClamped()) {
            m_brake_->sendClamp();
          }
          if (!f_brake_->checkClamped()) {
            f_brake_->sendClamp();
          }
          Thread::sleep(em_brakes_.brake_command_wait_time);
          m_brake_->checkHome();
          f_brake_->checkHome();

          m_brake_->checkBrakingFailure();
          f_brake_->checkBrakingFailure();
        } else {
           if (m_brake_->checkClamped()) {
            m_brake_->sendRetract();
          }
          if (f_brake_->checkClamped()) {
            f_brake_->sendRetract();
          }

          Thread::sleep(em_brakes_.brake_command_wait_time);
          m_brake_->checkHome();
          f_brake_->checkHome();

          m_brake_->checkAccFailure();
          f_brake_->checkAccFailure();
        }
        break;
      case data::State::kCalibrating:
        if (m_brake_->checkClamped()) {
          m_brake_->sendRetract();
        }
        if (f_brake_->checkClamped()) {
          f_brake_->sendRetract();
        }

        if (!m_brake_->checkClamped() && !f_brake_->checkClamped()) {
          em_brakes_.module_status = ModuleStatus::kReady;
          data_.setEmergencyBrakesData(em_brakes_);
        }

        Thread::sleep(em_brakes_.brake_command_wait_time);
        m_brake_->checkHome();
        f_brake_->checkHome();
        m_brake_->checkAccFailure();
        f_brake_->checkAccFailure();
        break;
      case data::State::kAccelerating:
        m_brake_->checkAccFailure();
        f_brake_->checkAccFailure();
        break;
      case data::State::kNominalBraking:
      case data::State::kEmergencyBraking:
        if (!m_brake_->checkClamped()) {
          m_brake_->sendClamp();
        }
        if (!f_brake_->checkClamped()) {
          f_brake_->sendClamp();
        }
        Thread::sleep(em_brakes_.brake_command_wait_time);
        m_brake_->checkHome();
        f_brake_->checkHome();

        m_brake_->checkBrakingFailure();
        f_brake_->checkBrakingFailure();
        break;
      case data::State::kFinished:
        if (tlm_data_.nominal_braking_command) {
          if (!m_brake_->checkClamped()) {
            m_brake_->sendClamp();
          }
          if (!f_brake_->checkClamped()) {
            f_brake_->sendClamp();
          }
          Thread::sleep(em_brakes_.brake_command_wait_time);
          m_brake_->checkHome();
          f_brake_->checkHome();
          m_brake_->checkBrakingFailure();
          f_brake_->checkBrakingFailure();
        } else {
          if (m_brake_->checkClamped()) {
            m_brake_->sendRetract();
          }
          if (f_brake_->checkClamped()) {
            f_brake_->sendRetract();
          }
          Thread::sleep(em_brakes_.brake_command_wait_time);
          m_brake_->checkHome();
          f_brake_->checkHome();
          m_brake_->checkAccFailure();
          f_brake_->checkAccFailure();
        }
        break;
      default:
        break;
    }
  }
  log_.INFO("Brakes", "Thread shutting down");
}

Main::~Main()
{
  delete f_brake_;
  delete m_brake_;
}

}  // namespace embrakes
}  // namespace hyped
