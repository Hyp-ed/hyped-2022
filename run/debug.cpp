#include <fstream>
#include <functional>
#include <iostream>
#include <memory>

#include <brakes/main.hpp>
#include <navigation/main.hpp>
#include <propulsion/main.hpp>
#include <sensors/gpio_counter.hpp>
#include <sensors/main.hpp>
#include <state_machine/main.hpp>
#include <telemetry/main.hpp>
#include <utils/system.hpp>

namespace hyped::debug {

struct Option {
  std::string identifier;
  std::string description;
  std::function<void()> action;

  Option(std::string identifier, std::string description, std::function<void()> action)
      : identifier(identifier),
        description(description),
        action(action)
  {
  }
};

struct KeyenceInstance {
  sensors::GpioCounter sensor;
  std::string path;
  std::ofstream file;
};

class Debug {
 public:
  Debug();

  void run();

 private:
  static constexpr std::size_t kNumOptions = 2;
  data::Data &data_;
  utils::System &system_;
  const std::array<Option, kNumOptions> options_;

  void printOptions();

  void handleOption();

  void handleShutdown();

  void handleKeyence();
};

Debug::Debug()
    : data_(data::Data::getInstance()),
      system_(utils::System::getSystem()),
      options_{
        Option("shutdown", "Turn system off", std::bind(&Debug::handleShutdown, this)),
        Option("keyence", "Toggle keyence", std::bind(&Debug::handleKeyence, this)),
      }
{
}

void Debug::run()
{
  auto &system = utils::System::getSystem();
  while (system.isRunning()) {
    printOptions();
    handleOption();
  }
}

void Debug::printOptions()
{
  std::cout << "Choose an option:" << std::endl;
  for (auto &option : options_) {
    std::cout << " * " << option.identifier;
    std::cout << " - " << option.description;
    std::cout << std::endl;
  }
  std::cout << "> ";
}

void Debug::handleOption()
{
  std::string command;
  std::cin >> command;
  for (auto &option : options_) {
    if (option.identifier == command) {
      option.action();
      return;
    }
  }
  std::cout << "Command \"" << command << "\" not found" << std::endl;
}

void Debug::handleShutdown()
{
  system_.stop();
}

void Debug::handleKeyence()
{
}

}  // namespace hyped::debug

int main(int argc, char *argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  hyped::debug::Debug debug;
  debug.run();
}