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

bool FileReader::readFileData(ControllerMessage *messages, int len, const char *filepath)
{
  FILE *fp;
  fp = fopen(filepath, "r");

  if (fp == NULL) {
    log_.ERR("MOTOR", "Unable to open: %s", filepath);

    return false;
  } else {
    int m = 0;
    char line[250];
    while (fgets(line, static_cast<int>(sizeof(line) / sizeof(line[0])), fp) != NULL) {
      if (line[0] == '\n' || line[0] == '\0') {
      } else if (line[0] == '#') {
      } else if (line[0] == '>') {
        for (int i = 1; i < static_cast<int>(strlen(line)) - 1; i++) {
          messages[m].logger_output[i - 1] = line[i];
        }
      } else {
        std::string lineData[8];
        splitData(line, lineData);
        addData(lineData, messages[m].message_data);
        m++;
      }
    }
  }
  fclose(fp);
  return true;
}

void FileReader::splitData(std::basic_string<char> line, std::string lineData[])
{
  std::vector<std::string> tokens;
  std::stringstream check1(line);
  std::string intermediate;
  while (getline(check1, intermediate, ' ')) {
    tokens.push_back(intermediate);
  }
  for (int i = 0; i < (signed)tokens.size(); i++) {
    lineData[i] = tokens[i];
  }
}

void FileReader::addData(std::string lineData[], uint8_t *message_data)
{
  for (int i = 0; i < 8; i++) {
    message_data[i] = static_cast<uint8_t>(std::stoi(lineData[i], NULL, 16));
  }
}
}  // namespace motor_control
}  // namespace hyped
