#include "observer.hpp"
#include "repl.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

#include <sensors/main.hpp>

namespace hyped::debugging {

Repl::Repl()
    : system_(utils::System::getSystem()),
      log_("REPL", utils::System::getSystem().config_.log_level),
      data_(data::Data::getInstance())
{
  addQuitCommand();
  addHelpCommand();
}

void Repl::run()
{
  printAvailableCommands();
  while (system_.isRunning()) {
    readAndHandleCommand();
  }
}

std::optional<std::unique_ptr<Repl>> Repl::fromFile(const std::string &)
{
  const auto &system = utils::System::getSystem();
  utils::Logger log("REPL", system.config_.log_level);
  auto repl                   = std::make_unique<Repl>();
  const auto brake_pin_vector = brakes::Main::pinsFromFile(log, system.config_.brakes_config_path);
  if (!brake_pin_vector) {
    log.error("failed to read Brake pins");
    return std::nullopt;
  }
  if (system.config_.use_fake_brakes) {
    repl->addFakeBrakeCommands(1);
    repl->addFakeBrakeCommands(2);
  }
  return repl;
}

void Repl::printAvailableCommands()
{
  std::cout << "Available options:" << std::endl;
  for (auto &option : commands_) {
    std::cout << " * `" << option.identifier;
    std::cout << "' - " << option.description;
    std::cout << std::endl;
  }
}

void Repl::readAndHandleCommand()
{
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

void Repl::addCommand(const Command &command)
{
  commands_.push_back(command);
  handlers_by_identifier_.emplace(command.identifier, command.handler);
}

void Repl::addQuitCommand()
{
  addCommand({"quit", "Turn the system off", [this]() { system_.stop(); }});
}

void Repl::addHelpCommand()
{
  addCommand({"help", "Print all options", [this]() { printAvailableCommands(); }});
}

void Repl::addBrakeCommands(const brakes::BrakePins &pins, const uint32_t id)
{
  const auto brake
    = std::make_shared<brakes::Brake>(pins.command_pin, pins.button_pin, static_cast<uint8_t>(id));
  {
    Command command;
    std::stringstream identifier;
    identifier << "brake " << id << " engage";
    command.identifier = identifier.str();
    std::stringstream description;
    description << "Engage the brake " << id;
    description << " with command pin " << pins.command_pin;
    description << " and button pin " << pins.button_pin;
    command.description = description.str();
    command.handler     = [brake, this]() { brake->engage(); };
    addCommand(command);
  }
  {
    Command command;
    std::stringstream identifier;
    identifier << "brake " << id << " retract";
    command.identifier = identifier.str();
    std::stringstream description;
    description << "Retract the brake " << id;
    description << " with command pin " << pins.command_pin;
    description << " and button pin " << pins.button_pin;
    command.description = description.str();
    command.handler     = [brake, this]() { brake->retract(); };
    addCommand(command);
  }
  {
    Command command;
    std::stringstream identifier;
    identifier << "brake " << id << " query";
    command.identifier = identifier.str();
    std::stringstream description;
    description << "Get status of the brake " << id;
    description << " with command pin " << pins.command_pin;
    description << " and button pin " << pins.button_pin;
    description << " is engage";
    command.description = description.str();
    command.handler     = [brake, id, this]() {
      if (brake->isEngaged()) {
        log_.info("brake %u is engaged", id);
      } else {
        log_.info("brake %u is retracted", id);
      }
    };
    addCommand(command);
  }
}

void Repl::addFakeBrakeCommands(const uint32_t id)
{
  const auto brake = std::make_shared<brakes::FakeBrake>(static_cast<uint8_t>(id));
  {
    Command command;
    std::stringstream identifier;
    identifier << "brake " << id << " engage";
    command.identifier = identifier.str();
    std::stringstream description;
    description << "Engage the fake brake " << id;
    command.description = description.str();
    command.handler     = [brake, this]() { brake->engage(); };
    addCommand(command);
  }
  {
    Command command;
    std::stringstream identifier;
    identifier << "brake " << id << " retract";
    command.identifier = identifier.str();
    std::stringstream description;
    description << "Retract the fake brake " << id;
    command.description = description.str();
    command.handler     = [brake, this]() { brake->retract(); };
    addCommand(command);
  }
  {
    Command command;
    std::stringstream identifier;
    identifier << "brake " << id << " query";
    command.identifier = identifier.str();
    std::stringstream description;
    description << "Get status of the fake brake " << id;
    command.description = description.str();
    command.handler     = [brake, id, this]() {
      if (brake->isEngaged()) {
        log_.info("brake %u is engaged", id);
      } else {
        log_.info("brake %u is retracted", id);
      }
    };
    addCommand(command);
  }
}

}  // namespace hyped::debugging
