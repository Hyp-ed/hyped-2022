#include "main.hpp"

namespace hyped::brakes {

Main::Main()
    : utils::concurrent::Thread(
      utils::Logger("BRAKES", utils::System::getSystem().config_.log_level_brakes)),
      sys_(utils::System::getSystem()),
      data_(data::Data::getInstance())
{
  if (sys_.config_.use_fake_brakes) {
    m_brake_ = new FakeStepper(log_, 1);
    f_brake_ = new FakeStepper(log_, 2);
  } else {
    auto pins = pinsFromFile(sys_.config_.brakes_config_path);
    if (!pins) {
      log_.error("failed to initialise brakes");
      sys_.stop();
      return;
    }
    m_brake_ = new Stepper(pins->command_pins.at(0), pins->button_pins.at(0), log_, 1);
    f_brake_ = new Stepper(pins->command_pins.at(1), pins->button_pins.at(1), log_, 2);
  }
}

void Main::run()
{
  // Setting module status for state machine transition
  brakes_               = data_.getEmergencyBrakesData();
  brakes_.module_status = ModuleStatus::kInit;
  data_.setEmergencyBrakesData(brakes_);

  log_.info("Thread started");

  while (sys_.isRunning()) {
    // Get the current state of brakes, state machine and telemetry modules from data
    brakes_   = data_.getEmergencyBrakesData();
    sm_data_  = data_.getStateMachineData();
    tlm_data_ = data_.getTelemetryData();

    switch (sm_data_.current_state) {
      case data::State::kIdle:
      case data::State::kPreCalibrating:
        if (tlm_data_.nominal_braking_command) {
          if (!m_brake_->checkClamped()) { m_brake_->sendClamp(); }
          if (!f_brake_->checkClamped()) { f_brake_->sendClamp(); }
          Thread::sleep(data::EmergencyBrakes::kBrakeCommandWaitTime);
          m_brake_->checkHome();
          f_brake_->checkHome();

          m_brake_->checkBrakingFailure();
          f_brake_->checkBrakingFailure();
        } else {
          if (m_brake_->checkClamped()) { m_brake_->sendRetract(); }
          if (f_brake_->checkClamped()) { f_brake_->sendRetract(); }

          Thread::sleep(data::EmergencyBrakes::kBrakeCommandWaitTime);
          m_brake_->checkHome();
          f_brake_->checkHome();

          m_brake_->checkAccFailure();
          f_brake_->checkAccFailure();
        }
        break;
      case data::State::kCalibrating:
        if (m_brake_->checkClamped()) { m_brake_->sendRetract(); }
        if (f_brake_->checkClamped()) { f_brake_->sendRetract(); }

        if (!m_brake_->checkClamped() && !f_brake_->checkClamped()) {
          brakes_.module_status = ModuleStatus::kReady;
          data_.setEmergencyBrakesData(brakes_);
        }

        Thread::sleep(data::EmergencyBrakes::kBrakeCommandWaitTime);
        m_brake_->checkHome();
        f_brake_->checkHome();
        m_brake_->checkAccFailure();
        f_brake_->checkAccFailure();
        break;
      case data::State::kAccelerating:
      case data::State::kCruising:
        m_brake_->checkAccFailure();
        f_brake_->checkAccFailure();
        break;
      case data::State::kNominalBraking:
      case data::State::kEmergencyBraking:
      case data::State::kInvalid:
        if (!m_brake_->checkClamped()) { m_brake_->sendClamp(); }
        if (!f_brake_->checkClamped()) { f_brake_->sendClamp(); }
        Thread::sleep(data::EmergencyBrakes::kBrakeCommandWaitTime);
        m_brake_->checkHome();
        f_brake_->checkHome();

        m_brake_->checkBrakingFailure();
        f_brake_->checkBrakingFailure();
        break;
      case data::State::kFinished:
        if (tlm_data_.nominal_braking_command) {
          if (!m_brake_->checkClamped()) { m_brake_->sendClamp(); }
          if (!f_brake_->checkClamped()) { f_brake_->sendClamp(); }
          Thread::sleep(data::EmergencyBrakes::kBrakeCommandWaitTime);
          m_brake_->checkHome();
          f_brake_->checkHome();
          m_brake_->checkBrakingFailure();
          f_brake_->checkBrakingFailure();
        } else {
          if (m_brake_->checkClamped()) { m_brake_->sendRetract(); }
          if (f_brake_->checkClamped()) { f_brake_->sendRetract(); }
          Thread::sleep(data::EmergencyBrakes::kBrakeCommandWaitTime);
          m_brake_->checkHome();
          f_brake_->checkHome();
          m_brake_->checkAccFailure();
          f_brake_->checkAccFailure();
        }
        break;
      case data::State::kReady:
      case data::State::kFailureStopped:
        break;
    }
  }
  log_.info("Thread shutting down");
}

Main::~Main()
{
  delete f_brake_;
  delete m_brake_;
}

}  // namespace hyped::brakes
