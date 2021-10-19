#pragma once

#include <string>

#include <utils/config.hpp>
#include <utils/logger.hpp>

namespace hyped {

namespace telemetry {

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
  int sockfd_;
  const std::string *kPort;
  const std::string *kServerIP;
};

}  // namespace telemetry
}  // namespace hyped
