/*
 * Author: Iain Macpherson
 * Organisation: HYPED
 * Date: 11/03/2019
 * Description: Main class to read motor configuration data registers from a txt file
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

#ifndef PROPULSION_FILE_READER_HPP_
#define PROPULSION_FILE_READER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sstream>
#include <vector>
#include <string>

#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "propulsion/controller_interface.hpp"

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
  static bool readFileData(ControllerMessage messages[], int len, const char* filepath);

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
  static void addData(std::string lineData[], uint8_t* message_data);
};
}}  // namespace hyped::motor_control
#endif  // PROPULSION_FILE_READER_HPP_
