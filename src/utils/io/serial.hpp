#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include <vector>

#include <utils/logger.hpp>
namespace hyped::utils::io {

enum class BaudRate { kB300, kB600, kB1200, kB2400, kB4800, kB9600, kB14400, kB19200, kB28800 };
class SerialProtocol {
 public:
  SerialProtocol(std::string serial, BaudRate baud_rate);
  ~SerialProtocol();

  void configureTermios();

  bool serialAvailable();

  void readData(std::vector<uint8_t> &buffer);
  void writeData(const std::vector<uint8_t> &buffer);

 private:
  std::string serial_device_;
  int serial_;
  BaudRate baud_rate_;
  utils::Logger log_;

  std::vector<char> readBuffer_;
  unsigned char readBufferSize_B_;
};
};  // namespace hyped::utils::io