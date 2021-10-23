#include "file_reader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include <utils/logger.hpp>

namespace hyped {
namespace motor_control {
utils::Logger log_(true, 0);

bool FileReader::readFileData(ControllerMessage *messages, const char *filepath)
{
  std::ifstream file(filepath);
  if (!file.is_open()) {
    log_.ERR("MOTOR", "Unable to open: %s", filepath);
    return false;
  }
  int m = 0;
  std::string line;
  while (std::getline(file, line)) {
    switch (line.at(0)) {
      case '\n':
      case '\0':
      case '#':
        break;
      case '>':
        std::copy(line.begin() + 1, line.end(), messages[m].logger_output);
        break;
      default:
        const auto line_data = splitData(line);
        if (line_data.size() != 8) {
          log_.ERR("MOTOR", "Error while reading %s. Expected exactly 8 tokens, found %u", filepath,
                   line_data.size());
        }
        addData(line_data, messages[m].message_data);
        ++m;
        break;
    }
  }
  return true;
}

std::vector<std::string> FileReader::splitData(const std::string line)
{
  std::vector<std::string> tokens;
  std::stringstream input_stream(line);
  std::string token;
  while (std::getline(input_stream, token, ' ')) {
    tokens.push_back(token);
  }
  return tokens;
}

void FileReader::addData(const std::vector<std::string> line_data, uint8_t *message_data)
{
  for (int i = 0; i < 8; ++i) {  // stores 8 bytes to be sent to motor controller
    message_data[i] = static_cast<uint8_t>(std::stoi(line_data.at(i), NULL, 16));
  }
}
}  // namespace motor_control
}  // namespace hyped
