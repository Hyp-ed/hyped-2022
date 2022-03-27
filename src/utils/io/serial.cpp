#include "serial.hpp"

#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

namespace hyped::utils::io {

SerialProtocol::SerialProtocol(const std::string serial, const BaudRate baud_rate,
                               utils::Logger &log)
    : serial_device_(serial),
      baud_rate_(baud_rate),
      log_(log)
{
  configureTermios();
}

SerialProtocol::~SerialProtocol()
{
  close(serial_);
};

bool SerialProtocol::serialAvailable()
{
  return serial_ > 0;
}

void SerialProtocol::configureTermios()
{
  serial_ = open(serial_device_.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

  if (serial_ == -1) {
    log_.error("Unable to open serial device: %s", serial_device_.c_str());
    return;
  }

  struct termios options;
  tcgetattr(serial_, &options);
  options.c_cflag = CS8 | CLOCAL | CREAD;
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  switch (baud_rate_) {
    case BaudRate::kB_300:
      cfsetispeed(&options, B300);
      cfsetospeed(&options, B300);
      break;
    case BaudRate::kB_600:
      cfsetispeed(&options, B600);
      cfsetospeed(&options, B600);
      break;
    case BaudRate::kB_1200:
      cfsetispeed(&options, B1200);
      cfsetospeed(&options, B1200);
      break;
    case BaudRate::kB_2400:
      cfsetispeed(&options, B2400);
      cfsetospeed(&options, B2400);
      break;
    case BaudRate::kB_4800:
      cfsetispeed(&options, B4800);
      cfsetospeed(&options, B4800);
      break;
    case BaudRate::kB_9600:
      cfsetispeed(&options, B9600);
      cfsetospeed(&options, B9600);
      break;
    case BaudRate::kB_19200:
      cfsetispeed(&options, B19200);
      cfsetospeed(&options, B19200);
      break;
    default:
      cfsetispeed(&options, B9600);
      cfsetospeed(&options, B9600);
      break;
  }
  tcflush(serial_, TCIFLUSH);
  tcsetattr(serial_, TCSANOW, &options);
}

void SerialProtocol::readData(std::vector<uint8_t> &data)
{
  data.clear();
  if (!serialAvailable()) {
    log_.error("Serial device not available for read.");
    return;
  }

  size_t n = read(serial_, &readBuffer_[0], readBufferSize_B_);

  if (n < 0) {
    log_.error("Error reading from serial device.");
    return;
  }

  copy(readBuffer_.begin(), readBuffer_.begin() + n, back_inserter(data));
  return;
}

void SerialProtocol::writeData(const std::vector<uint8_t> &data)
{
  if (!serialAvailable()) {
    log_.error("Serial device not available for write.");
    return;
  }

  int write_result = write(serial_, data.data(), data.size());

  if (write_result == -1) {
    log_.error("Error writing to serial device.");
    return;
  }
}
}  // namespace hyped::utils::io