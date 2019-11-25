/*
 * Author: Neil Weidinger
 * Organisation: HYPED
 * Date: March 2019
 * Description:
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include "telemetry/client.hpp"
#include "utils/system.hpp"

namespace hyped {
namespace telemetry {

Client::Client(Logger& log)
  : Client {log, *utils::System::getSystem().config}
{}

Client::Client(Logger& log, const utils::Config& config)
  : log_ {log},
    kPort {config.telemetry.Port.c_str()},
    kServerIP {config.telemetry.IP.c_str()}
{
  log_.DBG("Telemetry", "Client object created");
}

bool Client::connect()
{
  log_.INFO("Telemetry", "Beginning process to connect to server");

  struct addrinfo hints;
  struct addrinfo* server_info;  // contains possible addresses to connect to according to hints

  // set up criteria for type of address we want to connect to
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  // get possible addresses we can connect to
  int return_val;
  if ((return_val = getaddrinfo(kServerIP, kPort, &hints, &server_info)) != 0) {
    log_.ERR("Telemetry", "%s", gai_strerror(return_val));
    throw std::runtime_error{"Failed getting possible addresses"};
  }

  // get a socket file descriptor
  sockfd_ = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
  if (sockfd_ == -1) {
    log_.ERR("Telemetry", "%s", strerror(errno));
    throw std::runtime_error{"Failed getting socket file descriptor"};
  }

  // connect socket to server
  if (::connect(sockfd_, server_info->ai_addr, server_info->ai_addrlen) == -1) {
    close(sockfd_);
    log_.ERR("Telemetry", "%s", strerror(errno));
    throw std::runtime_error{"Failed connecting to socket (couldn't connect to server)"};
  }

  log_.INFO("Telemetry", "Connected to server");

  return true;
}

Client::~Client()
{
  close(sockfd_);
}

bool Client::sendData(std::string message)
{
  log_.DBG3("Telemetry", "Starting to send message to server");

  message.append("\n");

  int payload_length = message.length();

  // send payload
  if (send(sockfd_, message.c_str(), payload_length, 0) == -1) {
    return false;
  }

  log_.DBG3("Telemetry", "Finished sending message to server");

  return true;
}

std::string Client::receiveData()
{
  log_.DBG1("Telemetry", "Waiting to receive from server");

  char header[8];

  // receive header
  if (recv(sockfd_, header, 8, 0) == -1) {
    log_.ERR("Telemetry", "Error receiving header");
    throw std::runtime_error{"Error receiving header"};  // NOLINT
  }

  int payload_length = strtol(header, NULL, 0);
  char buffer[1024];  // power of 2 because apparently it's better for networking
  memset(buffer, 0, sizeof(buffer));  // fill with 0's so null terminated by default

  // receive payload
  if (recv(sockfd_, buffer, payload_length, 0) == -1) {
    log_.ERR("Telemetry", "Error receiving payload");
    throw std::runtime_error{"Error receiving payload"};  // NOLINT
  }

  log_.DBG1("Telemetry", "Finished receiving from server");

  return std::string(buffer);
}

}  // namespace client
}  // namespace hyped
