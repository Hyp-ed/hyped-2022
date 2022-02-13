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
  struct Config {
    bool use_keyence;
  };
  struct Command {
    std::string identifier;
    std::string description;
    Handler handler;
  };
  Repl(const Config &config);

  void run();

  static std::optional<Repl> fromFile(const std::string &path);

 private:
  utils::System &system_;
  data::Data &data_;
  const Config config_;
  std::vector<Command> commands_;
  std::unordered_map<std::string, Handler> handlers_by_identifier_;

  void printAvailableCommands();

  void readAndHandleCommand();

  void handleShutdown();

  static std::optional<Config> readConfig(utils::Logger &log, const std::string &path);
};

}  // namespace hyped::debugging
