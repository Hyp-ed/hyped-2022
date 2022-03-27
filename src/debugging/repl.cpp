#include "observer.hpp"
#include "repl.hpp"

#include <fstream>
#include <iostream>

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

std::optional<std::unique_ptr<Repl>> Repl::fromFile(const std::string &)
{
  return std::make_unique<Repl>();
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

}  // namespace hyped::debugging
