#include "UI.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace hyped::demo_state_machine {

UI::UI()
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

void UI::run()
{
  printAvailableCommands();
  while (system_.isRunning()) {
    printCurrentState();
    readAndHandleCommand();
  }
}

void UI::printCurrentState()
{
  auto current_state = data_.getStateMachineData().current_state;
  std::cout << "Current State: " << ::hyped::data::stateToString(current_state)->c_str()
            << std::endl;
  std::cout << "------------------------------" << std::endl;
}

void UI::printAvailableCommands()
{
  std::cout << "Available options:" << std::endl;
  for (auto &option : commands_) {
    std::cout << " * `" << option.identifier;
    std::cout << "' - " << option.description;
    std::cout << std::endl;
  }
}

void UI::readAndHandleCommand()
{
  auto current_state = data_.getStateMachineData().current_state;
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

void UI::addCommand(const Command &command)
{
  commands_.push_back(command);
  handlers_by_identifier_.emplace(command.identifier, command.handler);
}

void UI::addQuitCommand()
{
  addCommand({"quit", "Turn the system off", [this]() { system_.stop(); }});
}

void UI::addHelpCommand()
{
  addCommand({"help", "Print all options", [this]() { printAvailableCommands(); }});
}

void UI::giveCalibrationCommand()
{
  telemetry_data_                   = data_.getTelemetryData();
  telemetry_data_.calibrate_command = true;
  data_.setTelemetryData(telemetry_data_);
}

void UI::addCalibrationCommand()
{
  addCommand(
    {"calibrate", "Allows pod to enter Calibrating", [this]() { giveCalibrationCommand(); }});
}

void UI::giveLaunchCommand()
{
  telemetry_data_                = data_.getTelemetryData();
  telemetry_data_.launch_command = true;
  data_.setTelemetryData(telemetry_data_);
}

void UI::addLaunchCommand()
{
  addCommand({"launch", "Allows pod to enter Accelerating", [this]() { giveLaunchCommand(); }});
}

void UI::giveBrakingCommand()
{
  telemetry_data_                        = data_.getTelemetryData();
  telemetry_data_.emergency_stop_command = true;
  data_.setTelemetryData(telemetry_data_);
}

void UI::addBrakingCommand()
{
  addCommand({"brake", "Allows pod to enter Pre-Braking", [this]() { giveBrakingCommand(); }});
}

void UI::giveShutDownCommand()
{
  telemetry_data_                  = data_.getTelemetryData();
  telemetry_data_.shutdown_command = true;
  data_.setTelemetryData(telemetry_data_);
}

void UI::addShutDownCommand()
{
  addCommand({"shut", "Allows pod to turn off", [this]() { giveShutDownCommand(); }});
}

}  // namespace hyped::demo_state_machine
