#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include <vector>

#include <utils/logger.hpp>
namespace hyped::utils::io {

enum class SerialState {
  kNoSerial = 0,
  kSuccess  = 1,
  // There is not (enough) data to process
  kNoData = 2,
  // The object is being received but the buffer doesn't have all the data
  kWaitingForData = 3,
  // The size of the received payload doesn't match the expected size
  kInvalidSize = 4,
  // The object was received but it is not the same as one sent
  kInvalidChecksum = 5
};

enum class BaudRate {
  kB_300,
  kB_600,
  kB_1200,
  kB_2400,
  kB_4800,
  kB_9600,
  kB_14400,
  kB_19200,
  kB_28800
};
class SerialProtocol {
 public:
  SerialProtocol(std::string serial, BaudRate baud_rate, Logger &log);
  ~SerialProtocol();

  void configureTermios();

  bool serialAvailable();

  void readData(std::vector<uint8_t> &buffer);
  void writeData(const std::vector<uint8_t> &buffer);

 private:
  std::string serial_device_;
  int serial_;
  BaudRate baud_rate_;
  utils::Logger &log_;

  std::vector<char> readBuffer_;
  unsigned char readBufferSize_B_;
};
};  // namespace hyped::utils::io