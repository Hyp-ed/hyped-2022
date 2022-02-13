#include "repl.hpp"

#include <fstream>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

namespace hyped::debugging {

Repl::Repl(const Repl::Config &config)
    : system_(utils::System::getSystem()),
      data_(data::Data::getInstance()),
      config_(config)
{
  commands_.push_back({"shutdown", "Turn the system off", std::bind(&Repl::handleShutdown, *this)});
  for (const auto &command : commands_) {
    handlers_by_identifier_.emplace(command.identifier, command.handler);
  }
}

void Repl::run()
{
  while (system_.isRunning()) {
    printAvailableCommands();
    readAndHandleCommand();
  }
}

std::optional<Repl> Repl::fromFile(const std::string &path)
{
  auto &system = utils::System::getSystem();
  utils::Logger log("REPL", system.config_.log_level_debugger);
  const auto config_optional = readConfig(log, path);
  if (!config_optional) {
    log.error("Failed to read config file at %s. Could not construct object.", path.c_str());
    return std::nullopt;
  }
  return Repl(*config_optional);
}

void Repl::printAvailableCommands()
{
  std::cout << "Available options:" << std::endl;
  for (auto &option : commands_) {
    std::cout << " * " << option.identifier;
    std::cout << " - " << option.description;
    std::cout << std::endl;
  }
  std::cout << "> ";
}

void Repl::readAndHandleCommand()
{
  std::string command;
  std::cin >> command;
  auto identifier_and_handler = handlers_by_identifier_.find(command);
  if (identifier_and_handler == handlers_by_identifier_.end()) {
    std::cout << "Unknown command: " << command << std::endl;
    return;
  }
  identifier_and_handler->second();
}

void Repl::handleShutdown()
{
  system_.stop();
}

std::optional<Repl::Config> Repl::readConfig(utils::Logger &log, const std::string &path)
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
  if (!document.HasMember("debugger")) {
    log.error("Missing required field 'debugger' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  const auto config_object = document["debugger"].GetObject();
  Config config;
  if (!config_object.HasMember("use_keyence")) {
    log.error("Missing required field 'use_keyence' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  config.use_keyence = config_object["use_keyence"].GetBool();
  return config;
}

}  // namespace hyped::debugging