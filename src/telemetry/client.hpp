#pragma once

#include <string>

#include <utils/config.hpp>
#include <utils/logger.hpp>

namespace hyped::telemetry {

class Client {
 public:
  explicit Client(utils::Logger &log);
  ~Client();
  bool connect();
  bool sendData(std::string message);
  std::string receiveData();

 private:
  Client(utils::Logger &log, const utils::Config &config);

  utils::Logger &log_;
  int socket_;
  const std::string port_;
  const std::string server_ip_;
};

}  // namespace hyped::telemetry
