#pragma once

#include "controller_interface.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sstream>
#include <string>
#include <vector>

#include <utils/logger.hpp>
#include <utils/system.hpp>

namespace hyped {
namespace motor_control {

using hyped::utils::Logger;
using hyped::utils::System;

class FileReader {
 public:
  /**
   * @brief read data from file and write into the array message.
   *
   * @param message
   * @param len - length of messages array
   */
  static bool readFileData(ControllerMessage messages[], int len, const char *filepath);

 private:
  /**
   * @brief helper function to split the line into strings containing the hex data and
   *        a string logger message.
   * @param line
   * @return const char*
   */
  static void splitData(std::basic_string<char> line, std::string lineData[]);
  /*
   * @brief helper function to add the split data to the message struct.
   *
   * @param str
   * @param message
   * @param len
   */
  static void addData(std::string lineData[], uint8_t *message_data);
};
}  // namespace motor_control
}  // namespace hyped