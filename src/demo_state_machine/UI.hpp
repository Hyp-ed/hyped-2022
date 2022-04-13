#pragma once

#include <atomic>
#include <functional>
#include <optional>
#include <string>

#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::demo_state_machine {

class UI {
 public:
  using Handler = std::function<void(void)>;
  void printAvailableCommands();
  void printCurrentState();
  void readAndHandleCommand();

  struct Command {
    std::string identifier;
    std::string description;
    Handler handler;
  };

  UI();

  void run();

 private:
  utils::System &system_;
  utils::Logger log_;
  data::Data &data_;
  std::vector<Command> commands_;
  std::unordered_map<std::string, Handler> handlers_by_identifier_;

  data::Telemetry telemetry_data_;

  void addCommand(const Command &command);
  void addQuitCommand();
  void addHelpCommand();
  void addCalibrationCommand();
  void addLaunchCommand();
  void addBrakingCommand();
  void addShutDownCommand();

  void giveCalibrationCommand();
  void giveLaunchCommand();
  void giveBrakingCommand();
  void giveShutDownCommand();
};

}  // namespace hyped::demo_state_machine
