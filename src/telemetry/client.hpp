#pragma once

#include <string>
#include <utils/config.hpp>
#include <utils/logger.hpp>

namespace hyped {

using utils::Config;
using utils::Logger;

namespace telemetry {

class Client {
 public:
  explicit Client(Logger &log);
  ~Client();
  bool connect();
  bool sendData(std::string message);
  std::string receiveData();

 private:
  Client(Logger &log, const utils::Config &config);

  Logger &log_;
  int sockfd_;
  const char *kPort;
  const char *kServerIP;
};

}  // namespace telemetry
}  // namespace hyped
