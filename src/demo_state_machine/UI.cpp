#include "UI.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace hyped::demo_state_machine {

Ui::Ui()
    : system_(utils::System::getSystem()),
      log_("UI", utils::System::getSystem().config_.log_level),
      data_(data::Data::getInstance())
{
  addCalibrationCommand();
  addLaunchCommand();
  addBrakingCommand();
  addShutDownCommand();
  addHelpCommand();
  addQuitCommand();
}

void Ui::run()
{
  printAvailableCommands();
  while (system_.isRunning()) {
    readAndHandleCommand();
  }
}

void Ui::printAvailableCommands()
{
  std::cout << "------------------------------" << std::endl;
  std::cout << "Available options:" << std::endl;
  for (auto &option : commands_) {
    std::cout << " * `" << option.identifier;
    std::cout << "' - " << option.description;
    std::cout << std::endl;
  }
  std::cout << "------------------------------" << std::endl;
}

void Ui::readAndHandleCommand()
{
  const auto current_state = data_.getStateMachineData().current_state;
  if (current_state == data::State::kPreCalibrating || current_state == data::State::kReady
      || current_state == data::State::kAccelerating || current_state == data::State::kCruising
      || current_state == data::State::kFinished) {
    std::cout << "> ";
    std::string command;
    std::getline(std::cin, command);
    auto identifier_and_handler = handlers_by_identifier_.find(command);
    if (identifier_and_handler == handlers_by_identifier_.end()) {
      std::cout << "Unknown command: " << command << std::endl;
      return;
    }
    identifier_and_handler->second();
  }
}

void Ui::addCommand(const Command &command)
{
  commands_.push_back(command);
  handlers_by_identifier_.emplace(command.identifier, command.handler);
}

void Ui::addQuitCommand()
{
  addCommand({"quit", "Turn the system off", [this]() { system_.stop(); }});
}

void Ui::addHelpCommand()
{
  addCommand({"help", "Print all options", [this]() { printAvailableCommands(); }});
}

void Ui::giveCalibrationCommand()
{
  telemetry_data_                   = data_.getTelemetryData();
  telemetry_data_.calibrate_command = true;
  data_.setTelemetryData(telemetry_data_);
}

void Ui::addCalibrationCommand()
{
  addCommand(
    {"calibrate", "Allows pod to enter Calibrating", [this]() { giveCalibrationCommand(); }});
}

void Ui::giveLaunchCommand()
{
  telemetry_data_                = data_.getTelemetryData();
  telemetry_data_.launch_command = true;
  data_.setTelemetryData(telemetry_data_);
}

void Ui::addLaunchCommand()
{
  addCommand({"launch", "Allows pod to enter Accelerating", [this]() { giveLaunchCommand(); }});
}

void Ui::giveBrakingCommand()
{
  telemetry_data_                        = data_.getTelemetryData();
  telemetry_data_.emergency_stop_command = true;
  data_.setTelemetryData(telemetry_data_);
}

void Ui::addBrakingCommand()
{
  addCommand({"brake", "Allows pod to enter Pre-Braking", [this]() { giveBrakingCommand(); }});
}

void Ui::giveShutDownCommand()
{
  telemetry_data_                  = data_.getTelemetryData();
  telemetry_data_.shutdown_command = true;
  data_.setTelemetryData(telemetry_data_);
}

void Ui::addShutDownCommand()
{
  addCommand({"shut", "Allows pod to turn off", [this]() { giveShutDownCommand(); }});
}

}  // namespace hyped::demo_state_machine
