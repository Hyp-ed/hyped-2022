#pragma once

#include <optional>
#include <string>

#include <utils/logger.hpp>

namespace hyped::telemetry {

class Client {
 public:
  struct Config {
    std::string port;
    std::string server_ip;
  };
  ~Client();
  bool connect();
  bool sendData(std::string message);
  std::string receiveData();
  static std::optional<Client> fromFile(const std::string &path);

 private:
  Client(utils::Logger log, const Config &config);
  utils::Logger log_;
  const Config config_;
  int socket_;
};

}  // namespace hyped::telemetry
