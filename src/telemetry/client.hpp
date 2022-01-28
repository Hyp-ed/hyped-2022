#pragma once

#include <memory>
#include <optional>
#include <string>

#include <utils/logger.hpp>

namespace hyped::telemetry {

class Client {
 public:
  struct Config {
    std::string server_port;
    std::string server_ip;
  };

  Client(utils::Logger log, const Config &config);
  ~Client();
  static std::unique_ptr<Client> fromFile(const std::string &path);
  static std::optional<Config> readConfig(utils::Logger &log, const std::string &path);

  bool connect();
  bool sendData(std::string message);
  std::string receiveData();

 private:
  utils::Logger log_;
  const Config config_;
  int socket_;
};

}  // namespace hyped::telemetry
