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

#include <google/protobuf/util/delimited_message_util.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include "telemetry/client.hpp"
#include "utils/system.hpp"

namespace hyped {
namespace telemetry {

Client::Client(Logger& log)
  : Client {log, *utils::System::getSystem().config}
{}

Client::Client(Logger& log, const utils::Config& config)
  : log_ {log},
    signal_handler_ {},
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

  socket_stream_in_ = new google::protobuf::io::FileInputStream(sockfd_);
  socket_stream_out_ = new google::protobuf::io::FileOutputStream(sockfd_);
  return true;
}

Client::~Client()
{
  delete socket_stream_out_;
  delete socket_stream_in_;
  close(sockfd_);
}

bool Client::sendData(telemetry_data::ClientToServer message)
{
  using google::protobuf::util::SerializeDelimitedToZeroCopyStream;
  log_.DBG3("Telemetry", "Starting to send message to server");

  if (!SerializeDelimitedToZeroCopyStream(message, socket_stream_out_) || signal_handler_.gotSigPipeSignal()) {  // NOLINT
    throw std::runtime_error{"Error sending message"};
  }

  // we have to call Flush() here otherwise protobufs will buffer the file output stream
  // and the message will not be sent immediately like we would like
  socket_stream_out_->Flush();

  log_.DBG3("Telemetry", "Finished sending message to server");

  return true;
}

telemetry_data::ServerToClient Client::receiveData()
{
  using google::protobuf::util::ParseDelimitedFromZeroCopyStream;

  telemetry_data::ServerToClient messageFromServer;
  log_.DBG1("Telemetry", "Waiting to receive from server");

  if (!ParseDelimitedFromZeroCopyStream(&messageFromServer, socket_stream_in_, NULL) || signal_handler_.gotSigPipeSignal()) {  // NOLINT
    throw std::runtime_error{"Error receiving message"};
  }

  log_.DBG1("Telemetry", "Finished receiving from server");

  return messageFromServer;
}

}  // namespace client
}  // namespace hyped
