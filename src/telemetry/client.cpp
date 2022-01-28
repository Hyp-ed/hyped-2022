#include "client.hpp"

#include <netdb.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <utils/system.hpp>

namespace hyped::telemetry {

Client::Client(utils::Logger log, const Config &config) : log_(log), config_(config)
{
}

bool Client::connect()
{
  log_.info("Beginning process to connect to server");

  addrinfo hints;
  addrinfo *server_info;  // contains possible addresses to connect to according to hints

  // set up criteria for type of address we want to connect to
  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  // get possible addresses we can connect to
  const int error
    = getaddrinfo(config_.server_ip.c_str(), config_.server_port.c_str(), &hints, &server_info);
  if (error != 0) {
    log_.error("%s", gai_strerror(error));
    throw std::runtime_error{"Failed getting possible addresses"};
  }

  // get a socket file descriptor
  socket_ = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
  if (socket_ == -1) {
    log_.error("%s", strerror(errno));
    throw std::runtime_error{"Failed getting socket file descriptor"};
  }

  // connect socket to server
  if (::connect(socket_, server_info->ai_addr, server_info->ai_addrlen) == -1) {
    close(socket_);
    log_.error("%s", strerror(errno));
    throw std::runtime_error{"Failed connecting to socket (couldn't connect to server)"};
  }

  log_.info("Connected to server");

  return true;
}

Client::~Client()
{
  close(socket_);
}

bool Client::sendData(std::string message)
{
  log_.debug("Starting to send message to server");

  message.append("\n");

  int payload_length = message.length();

  // send payload
  if (send(socket_, message.c_str(), payload_length, 0) == -1) { return false; }

  log_.debug("Finished sending message to server");

  return true;
}

std::string Client::receiveData()
{
  log_.debug("Waiting to receive from server");

  char header[8];

  // receive header
  if (recv(socket_, header, 8, 0) == -1) { throw std::runtime_error{"Error receiving header"}; }

  int payload_length = strtol(header, NULL, 0);
  char buffer[1024];                  // power of 2 because apparently it's better for networking
  memset(buffer, 0, sizeof(buffer));  // fill with 0's so null terminated by default

  // receive payload
  if (recv(socket_, buffer, payload_length, 0) == -1) {
    throw std::runtime_error{"Error receiving payload"};
  }

  log_.debug("Finished receiving from server");

  return std::string(buffer);
}

std::unique_ptr<Client> Client::fromFile(const std::string &path)
{
  auto &system = utils::System::getSystem();
  utils::Logger log("CLIENT", system.config_.log_level_telemetry);
  const auto config_optional = readConfig(log, path);
  if (!config_optional) {
    log.error("Failed to read config filet at %s. Could not construct objects.", path.c_str());
    return nullptr;
  }
  auto config = *config_optional;
  return std::make_unique<Client>(log, config);
}

std::optional<Client::Config> Client::readConfig(utils::Logger &log, const std::string &path)
{
  std::ifstream input_stream(path);
  if (!input_stream.is_open()) {
    log.error("Failed to open config file at %s", path.c_str());
    return std::nullopt;
  }
  rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
  rapidjson::Document document;
  document.ParseStream(input_stream_wrapper);
  if (document.HasParseError()) {
    log.error("Failed to parse config file at %s", path.c_str());
    return std::nullopt;
  }
  if (!document.HasMember("telemetry")) {
    log.error("Missing required field 'telemetry' in configuration file at %s", path.c_str());
    return std::nullopt;
  }
  auto config_object = document["telemetry"].GetObject();
  Config config;
  if (!config_object.HasMember("server_ip")) {
    log.error("Missing required field 'telemetry.server_ip' in configuration filet at %s",
              path.c_str());
    return std::nullopt;
  }
  config.server_ip = config_object["server_ip"].GetString();
  if (!config_object.HasMember("server_port")) {
    log.error("Missing required field 'telemetry.server_port' in configuration filet at %s",
              path.c_str());
    return std::nullopt;
  }
  config.server_port = config_object["server_port"].GetString();
  return config;
}

}  // namespace hyped::telemetry
