#include "brake.hpp"
#include "main.hpp"

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
    magnetic_brake_ = std::make_unique<FakeBrake>(1);
    friction_brake_ = std::make_unique<FakeBrake>(2);
  } else {
    const auto pins = pinsFromFile(log_, sys_.config_.brakes_config_path);
    if (!pins) {
      log_.error("failed to initialise brakes");
      sys_.stop();
      return;
    }
    if (pins->size() != data::Brakes::kNumBrakes) {
      // we cast to 32-bit type to avoid cross compilation issues; on the BBB this will match size_t
      log_.error("found %u brakes but %u were expected", static_cast<uint32_t>(pins->size()),
                 static_cast<uint32_t>(data::Brakes::kNumBrakes));
      sys_.stop();
      return;
    }
    magnetic_brake_ = std::make_unique<Brake>(pins->at(0).command_pin, pins->at(0).button_pin, 1);
    friction_brake_ = std::make_unique<Brake>(pins->at(1).command_pin, pins->at(1).button_pin, 2);
  }
}

void Main::checkEngaged()
{
  if (!magnetic_brake_->isEngaged()) {
    log_.error("expected magnetic brake to be engaged");
    brakes_data_.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setBrakesData(brakes_data_);
  } else {
    brakes_data_.brakes_retracted[0] = true;
    data_.setBrakesData(brakes_data_);
  }
  if (!friction_brake_->isEngaged()) {
    log_.error("expected friction brake to be engaged");
    brakes_data_.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setBrakesData(brakes_data_);
  } else {
    brakes_data_.brakes_retracted[1] = true;
    data_.setBrakesData(brakes_data_);
  }
}

void Main::checkRetracted()
{
  if (magnetic_brake_->isEngaged()) {
    log_.error("expected magnetic brake to be retracted");
    brakes_data_.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setBrakesData(brakes_data_);
  } else {
    brakes_data_.brakes_retracted[0] = true;
    data_.setBrakesData(brakes_data_);
  }
  if (friction_brake_->isEngaged()) {
    log_.error("expected friction brake to be retracted");
    brakes_data_.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setBrakesData(brakes_data_);
  } else {
    brakes_data_.brakes_retracted[1] = true;
    data_.setBrakesData(brakes_data_);
  }
}

void Main::engage()
{
  magnetic_brake_->engage();
  friction_brake_->engage();
  sleep(data::Brakes::kBrakeCommandWaitTime);
  checkEngaged();
}

void Main::retract()
{
  magnetic_brake_->retract();
  friction_brake_->retract();
  sleep(data::Brakes::kBrakeCommandWaitTime);
  checkRetracted();
}

void Main::run()
{
  // Setting module status for state machine transition
  brakes_data_               = data_.getBrakesData();
  brakes_data_.module_status = data::ModuleStatus::kInit;
  data_.setBrakesData(brakes_data_);

  log_.info("Thread started");

  while (sys_.isRunning()) {
    // Get the current state of brakes, state machine and telemetry modules from data
    brakes_data_             = data_.getBrakesData();
    const auto current_state = data_.getStateMachineData().current_state;

    switch (current_state) {
      case data::State::kIdle:
      case data::State::kReady:
      case data::State::kFailureStopped:
        break;
      case data::State::kPreCalibrating:
      case data::State::kFinished: {
        const auto braking_command = data_.getTelemetryData().nominal_braking_command;
        if (braking_command) {
          engage();
        } else {
          retract();
        }
        break;
      }
      case data::State::kCalibrating:
        retract();
        brakes_data_.module_status = data::ModuleStatus::kReady;
        data_.setBrakesData(brakes_data_);
        break;
      case data::State::kAccelerating:
      case data::State::kCruising:
      case data::State::kPreBraking:
      case data::State::kFailurePreBraking:
        checkRetracted();
        break;
      case data::State::kNominalBraking:
      case data::State::kFailureBraking:
      case data::State::kInvalid:
        engage();
        break;
    }
  }
  log_.info("Thread shutting down");
}

std::optional<std::vector<BrakePins>> Main::pinsFromFile(utils::Logger &log,
                                                         const std::string &path)
{
  std::ifstream input_stream(path);
  if (!input_stream.is_open()) {
    log.error("Failed to open config file at %s", path.c_str());
    return std::nullopt;
  }
  rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
  rapidjson::Document document;
  document.ParseStream(input_stream_wrapper);
  if (document.HasParseError()) {
    log.error("Failed to parse config file at %s", path.c_str());
    return std::nullopt;
  }
  if (!document.HasMember("brakes")) {
    log.error("Missing required field 'brakes' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  auto config_object = document["brakes"].GetObject();
  std::vector<BrakePins> all_pins;
  if (!config_object.HasMember("pins")) {
    log.error("Missing required field 'brakes.pins' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  auto pin_array = config_object["pins"].GetArray();
  for (const auto &pin_object : pin_array) {
    BrakePins pins;
    if (!pin_object.HasMember("command_pin")) {
      log.error("Missing required field 'brakes.pins[%u].command_pin' in configuration file at %s",
                static_cast<uint32_t>(all_pins.size()), path.c_str());
      return std::nullopt;
    }
    pins.command_pin = pin_object["command_pin"].GetUint();
    if (!pin_object.HasMember("button_pin")) {
      log.error("Missing required field 'brakes.pins[%u].button_pin' in configuration file at %s",
                static_cast<uint32_t>(all_pins.size()), path.c_str());
      return std::nullopt;
    }
    pins.button_pin = pin_object["button_pin"].GetUint();
    all_pins.push_back(pins);
  }
  return all_pins;
}

}  // namespace hyped::brakes
