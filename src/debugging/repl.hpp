#pragma once

#include <atomic>
#include <functional>
#include <optional>
#include <string>

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
  Repl(utils::Logger log);

  void run();

  static std::optional<Repl> fromFile(const std::string &path);

 private:
  utils::System &system_;
  utils::Logger &log_;
  data::Data &data_;
  std::vector<Command> commands_;
  std::unordered_map<std::string, Handler> handlers_by_identifier_;

  void printAvailableCommands();
  void readAndHandleCommand();
  void handleShutdown();
};

}  // namespace hyped::debugging
