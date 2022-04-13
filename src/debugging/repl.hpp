#pragma once

#include <atomic>
#include <functional>
#include <optional>
#include <string>

#include <brakes/main.hpp>
#include <data/data.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped::debugging {

class Repl {
 public:
  using Handler = std::function<void(void)>;

  struct Command {
    std::string identifier;
    std::string description;
    Handler handler;
  };
  Repl();

  void run();

  static std::optional<std::unique_ptr<Repl>> fromFile(const std::string &path);

 private:
  utils::System &system_;
  utils::Logger log_;
  data::Data &data_;
  std::vector<Command> commands_;
  std::unordered_map<std::string, Handler> handlers_by_identifier_;

  void printAvailableCommands();
  void readAndHandleCommand();
  void addCommand(const Command &command);
  void addQuitCommand();
  void addHelpCommand();
  void addBrakeCommands(const brakes::BrakePins &pins, const uint32_t id);
  void addFakeBrakeCommands(const uint32_t id);
  void addDirectCanCommands();
};

}  // namespace hyped::debugging
