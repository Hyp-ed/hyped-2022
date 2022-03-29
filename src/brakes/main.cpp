#include "main.hpp"
#include "stepper.hpp"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

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
  brakes_.module_status = data::ModuleStatus::kInit;
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
          Thread::sleep(data::Brakes::kBrakeCommandWaitTime);
          m_brake_->checkHome();
          f_brake_->checkHome();

          m_brake_->checkBrakingFailure();
          f_brake_->checkBrakingFailure();
        } else {
          if (m_brake_->checkClamped()) { m_brake_->sendRetract(); }
          if (f_brake_->checkClamped()) { f_brake_->sendRetract(); }

          Thread::sleep(data::Brakes::kBrakeCommandWaitTime);
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
          brakes_.module_status = data::ModuleStatus::kReady;
          data_.setEmergencyBrakesData(brakes_);
        }

        Thread::sleep(data::Brakes::kBrakeCommandWaitTime);
        m_brake_->checkHome();
        f_brake_->checkHome();
        m_brake_->checkAccFailure();
        f_brake_->checkAccFailure();
        break;
      case data::State::kAccelerating:
      case data::State::kCruising:
      case data::State::kPreBraking:
      case data::State::kFailurePreBraking:
        m_brake_->checkAccFailure();
        f_brake_->checkAccFailure();
        break;
      case data::State::kNominalBraking:
      case data::State::kFailureBraking:
      case data::State::kInvalid:
        if (!m_brake_->checkClamped()) { m_brake_->sendClamp(); }
        if (!f_brake_->checkClamped()) { f_brake_->sendClamp(); }
        Thread::sleep(data::Brakes::kBrakeCommandWaitTime);
        m_brake_->checkHome();
        f_brake_->checkHome();

        m_brake_->checkBrakingFailure();
        f_brake_->checkBrakingFailure();
        break;
      case data::State::kFinished:
        if (tlm_data_.nominal_braking_command) {
          if (!m_brake_->checkClamped()) { m_brake_->sendClamp(); }
          if (!f_brake_->checkClamped()) { f_brake_->sendClamp(); }
          Thread::sleep(data::Brakes::kBrakeCommandWaitTime);
          m_brake_->checkHome();
          f_brake_->checkHome();
          m_brake_->checkBrakingFailure();
          f_brake_->checkBrakingFailure();
        } else {
          if (m_brake_->checkClamped()) { m_brake_->sendRetract(); }
          if (f_brake_->checkClamped()) { f_brake_->sendRetract(); }
          Thread::sleep(data::Brakes::kBrakeCommandWaitTime);
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

std::optional<Pins> Main::pinsFromFile(const std::string &path)
{
  std::ifstream input_stream(path);
  if (!input_stream.is_open()) {
    log_.error("Failed to open config file at %s", path.c_str());
    return std::nullopt;
  }
  rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
  rapidjson::Document document;
  document.ParseStream(input_stream_wrapper);
  if (document.HasParseError()) {
    log_.error("Failed to parse config file at %s", path.c_str());
    return std::nullopt;
  }
  if (!document.HasMember("brakes")) {
    log_.error("Missing required field 'brakes' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  auto config_object = document["brakes"].GetObject();
  Pins pins;
  if (!config_object.HasMember("command_pins")) {
    log_.error("Missing required field 'brakes.command_pins' in configuration file at %s",
               path.c_str());
    return std::nullopt;
  }
  auto command_pin_array = config_object["command_pins"].GetArray();
  if (command_pin_array.Size() != data::Brakes::kNumBrakes) {
    log_.error("Found %d command pins but %d were expected in configuration file at %s",
               command_pin_array.Size(), data::Brakes::kNumBrakes, path.c_str());
    return std::nullopt;
  }
  for (std::size_t i = 0; i < data::Brakes::kNumBrakes; ++i) {
    pins.command_pins.at(i) = static_cast<std::uint8_t>(command_pin_array[i].GetUint());
  }
  if (!config_object.HasMember("button_pins")) {
    log_.error("Missing required field 'brakes.button_pins' in configuration file at %s",
               path.c_str());
    return std::nullopt;
  }
  auto button_pin_array = config_object["button_pins"].GetArray();
  if (button_pin_array.Size() != data::Brakes::kNumBrakes) {
    log_.error("Found %d button pins but %d were expected in configuration file at %s",
               button_pin_array.Size(), data::Brakes::kNumBrakes, path.c_str());
    return std::nullopt;
  }
  for (std::size_t i = 0; i < data::Brakes::kNumBrakes; ++i) {
    pins.button_pins.at(i) = static_cast<std::uint8_t>(button_pin_array[i].GetUint());
  }
  return pins;
}

}  // namespace hyped::brakes
