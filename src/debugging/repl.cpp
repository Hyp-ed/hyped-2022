#include "observer.hpp"
#include "repl.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

#include <sensors/main.hpp>
#include <utils/io/can.hpp>

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

std::optional<std::unique_ptr<Repl>> Repl::fromFile(const std::string &path)
{
  const auto &system = utils::System::getSystem();
  utils::Logger log("REPL", system.config_.log_level);
  std::ifstream input_stream(path);
  if (!input_stream.is_open()) {
    log.error("failed to open config file at %s", path.c_str());
    return std::nullopt;
  }
  rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
  rapidjson::Document document;
  document.ParseStream(input_stream_wrapper);
  if (document.HasParseError()) {
    log.error("failed to parse config file at %s", path.c_str());
    return std::nullopt;
  }
  if (!document.HasMember("debugger")) {
    log.error("missing required field 'debugger' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  const auto config_object = document["debugger"].GetObject();
  if (!config_object.HasMember("use_direct_can")) {
    log.error("missing required field 'debugger.use_direct_can' in configuration file at %s",
              path.c_str());
    return std::nullopt;
  }
  const auto use_direct_can = config_object["use_direct_can"].GetBool();
  auto repl                 = std::make_unique<Repl>();
  if (use_direct_can) { repl->addDirectCanCommands(); }
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

void Repl::addDirectCanCommands()
{
  auto &can = utils::io::Can::getInstance();
  can.start();
  Command can_send_command;
  can_send_command.identifier  = "direct can send";
  can_send_command.description = "Send an aribtrary message, to be specified after, to the CAN bus";
  can_send_command.handler     = [this, &can]() {
    utils::io::can::Frame frame;
    frame.extended = false;
    std::cout << "CAN id (e.g. `3e'):" << std::endl;
    std::cin >> std::hex >> frame.id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Number of bytes to send (e.g. `7'):" << std::endl;
    uint32_t len;
    std::cin >> std::dec >> len;
    if (len > 8) {
      log_.error("found can message length %u which exceeds the maximum of 8 bytes", len);
      return;
    }
    frame.len = static_cast<uint32_t>(len);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Bytes to send (e.g. `ff";
    for (uint8_t i = 1; i < frame.len; ++i) {
      std::cout << " ff";
    }
    std::cout << "':" << std::endl;
    for (uint8_t i = 0; i < frame.len; ++i) {
      uint32_t value;
      std::cin >> std::hex >> value;
      if (value > UINT8_MAX) {
        log_.error("found value %u which exceeds the maximum of 255 for a single byte", value);
      }
      frame.data[i] = static_cast<uint8_t>(value);
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    can.send(frame);
  };
  addCommand(can_send_command);
}

}  // namespace hyped::debugging
