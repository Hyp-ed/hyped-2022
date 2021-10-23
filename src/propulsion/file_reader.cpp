/*
 * TODO(Iain): reimplement to recieve a path to a file and then iterate through all the
 *             messages to initialise them.
 */
#include "file_reader.hpp"

#include <string>
#include <vector>

namespace hyped {
namespace motor_control {
utils::Logger log_(true, 0);

bool FileReader::readFileData(ControllerMessage *messages, const char *filepath)
{
  FILE *current_file = fopen(filepath, "r");
  if (current_file == NULL) {
    log_.ERR("MOTOR", "Unable to open: %s", filepath);
    return false;
  }

  int m = 0;
  char line[250];
  while (fgets(line, static_cast<int>(sizeof(line) / sizeof(line[0])), current_file) != NULL) {
    if (line[0] == '\n' || line[0] == '\0' || line[0] == '#') { continue; }
    if (line[0] == '>') {
      for (int i = 1; i < static_cast<int>(strlen(line)) - 1; i++) {
        messages[m].logger_output[i - 1] = line[i];
      }
      continue;
    }
    std::string line_data[8];
    splitData(line, line_data);
    addData(line_data, messages[m].message_data);
    m++;
  }

  fclose(current_file);
  return true;
}

void FileReader::splitData(const std::basic_string<char> line, std::string line_data[])
{
  std::vector<std::string> tokens;
  std::stringstream check1(line);
  std::string intermediate;
  while (getline(check1, intermediate, ' ')) {
    tokens.push_back(intermediate);
  }
  for (int i = 0; i < (signed)tokens.size(); i++) {
    line_data[i] = tokens[i];
  }
}

void FileReader::addData(const std::string line_data[], uint8_t *message_data)
{
  for (int i = 0; i < 8; ++i) {  // stores 8 bytes to be sent to motor controller
    message_data[i] = static_cast<uint8_t>(std::stoi(line_data[i], NULL, 16));
  }
}
}  // namespace motor_control
}  // namespace hyped
