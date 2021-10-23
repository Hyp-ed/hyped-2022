#pragma once

#include "controller_interface.hpp"

#include <string>

namespace hyped {
namespace motor_control {

class FileReader {
 public:
  /**
   * @brief read data from file and write into the array message.
   *
   * @param message
   * @param len - length of messages array
   */
  static bool readFileData(ControllerMessage messages[], const char *filepath);

 private:
  /**
   * @brief helper function to split the line into strings containing the hex data and
   *        a string logger message.
   * @param line
   * @return const char*
   */
  static std::vector<std::string> splitData(const std::string line);
  /*
   * @brief helper function to add the split data to the message struct.
   *
   * @param str
   * @param message
   * @param len
   */
  static void addData(const std::vector<std::string> line_data, uint8_t *message_data);
};
}  // namespace motor_control
}  // namespace hyped
