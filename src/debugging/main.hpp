#pragma once

#include <optional>

#include <utils/logger.hpp>

namespace hyped::debugging {

class Main {
 public:
  struct Config {
    bool use_keyence;
  };
  static std::optional<Config> readConfig(utils::Logger &log, const std::string &path);
};

}  // namespace hyped::debugging
