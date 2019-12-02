/*
 * Authors: M. Kristien
 * Organisation: HYPED
 * Date: 3. April 2019
 * Description:
 *
 *    Copyright 2018 HYPED
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

#include "utils/config.hpp"
#include <cstdio>
#include <cstring>
#include <string>  // redundant includes to make linter stop complaining
#include <vector>
#include <sstream>
#include "utils/logger.hpp"
#include "utils/system.hpp"

namespace hyped {
namespace utils {

#define BUFFER_SIZE 250   // max length of a line in the confix file in characters

typedef void (Config::* Parser) (char* line);
struct ModuleEntry {
  Submodule label;
  char      name[20];   // no module name should exceed 20 characters
  Parser    parse;
};

/**
 * Update this table to register new config mapping. Each row corresponds
 * to a config file line parser. If a matching line is found, the line is
 * forwarded to the parser to update/load any corresponding configuration data.
 *
 * Column 0: Submodule enum entry
 * Column 1: A readable name as it appears in the configuration file
 * Column 2: An address of a Config:: member function that performs the line parsing.
 */
#define MAP_ENTRY(module) \
  {k##module , #module, &Config::Parse##module},

ModuleEntry module_map[] = {
  MODULE_LIST(MAP_ENTRY)
};

void Config::ParseNoModule(char* line)
{
  // does nothing
}

void Config::ParseNavigation(char* line)
{
  printf("nav %s\n", line);
}

void Config::ParseStateMachine(char* line)
{
  char* token = strtok(line, " ");
  if (strcmp(token, "Timeout") == 0) {
    char* value = strtok(NULL, " ");
    if (value) {
      statemachine.timeout = atoi(value);
    }
  }
}

void Config::ParseTelemetry(char* line)
{
  // just in case we get handed a null pointer
  if (!line) return;

  // convert char* line to c++ style string
  std::string cpp_line {line};

  std::istringstream iss(cpp_line);
  std::vector<std::string> tokens;

  for (std::string s; iss >> s;) {
    tokens.push_back(s);
  }

  if (tokens[0] == "IP") {
    telemetry.IP = tokens[1];
  } else if (tokens[0] == "Port") {
    telemetry.Port = tokens[1];
  }
}

void Config::ParseEmbrakes(char* line)
{
  char* token = strtok(line, " ");

  if (strcmp(token, "Command") == 0) {
    for (int i = 0; i < 4; i++) {
      char* value = strtok(NULL, ",");
      if (value) {
        embrakes.command[i] = atoi(value);
      }
    }
  }
  if (strcmp(token, "Button") == 0) {
    for (int i = 0; i < 4; i++) {
      char* value = strtok(NULL, ",");
      if (value) {
        embrakes.button[i] = atoi(value);
      }
    }
  }
}

void Config::ParseSensors(char* line)
{
  // EXAMPLE line parsing:
  // "char* strtok(line, delimiters)" splits the input line into parts using
  // characters from delimiters. The return value points to a valid split section.
  // To get another split section, call strtok again with NULL as the first
  // argument.
  // E.g. for line "IP 135.152.120.2", and you call these functions (in this order):
  // strtok(line, " ") returns string "IP"
  // strtok(NULL, " ") returns string "135.152.120.2"
  //
  // After this, the value can be converted from string to bool/int/string and
  // stored in the corresponding configuration field

  char* token = strtok(line, " ");

  if (strcmp(token, "ChipSelect") == 0) {
    for (int i = 0; i < data::Sensors::kNumImus; i++) {
      char* value = strtok(NULL, ",");
      if (value) {
      sensors.chip_select[i] = atoi(value);
      }
    }
  }

  if (strcmp(token, "KeyenceL") == 0) {
    char* value = strtok(NULL, " ");
    if (value) {
      sensors.KeyenceL = atoi(value);
    }
  }

  if (strcmp(token, "KeyenceR") == 0) {
    char* value = strtok(NULL, " ");
    if (value) {
      sensors.KeyenceR = atoi(value);
    }
  }

  if (strcmp(token, "Thermistor") == 0) {
    char* value = strtok(NULL, " ");
    if (value) {
      sensors.Thermistor = atoi(value);
    }
  }

  if (strcmp(token, "HPMaster") == 0) {
    char* value = strtok(NULL, " ");
    if (value) {
      sensors.hp_master= atoi(value);
    }
  }

  if (strcmp(token, "HPSSR") == 0) {
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      char* value = strtok(NULL, ",");
      if (value) {
        sensors.HPSSR[i] = atoi(value);
      }
    }
  }

  if (strcmp(token, "IMDOut") == 0) {
    char* value = strtok(NULL, " ");
    if (value) {
      sensors.IMDOut = atoi(value);
    }
  }

  if (strcmp(token, "Embrakes") == 0) {
    char* value = strtok(NULL, " ");
    if (value) {
      sensors.embrakes = atoi(value);
    }
  }
}


const char config_dir_name[] = "configurations/";

void Config::readFile(char* config_file) {
  char file_name[BUFFER_SIZE];
  std::strcpy(file_name, config_dir_name);
  std::strcpy(file_name+std::strlen(config_dir_name), config_file);
  // load config file, parse it into data structure
  FILE* file = fopen(file_name, "r");
  if (!file) {
    log_.ERR("CONFIG", "no configuration file %s found, exiting", file_name);
    return;
  }

  log_.INFO("CONFIG", "loading configuration file %s", file_name);

  // allocate line buffer, read and parse file line by line
  char line[BUFFER_SIZE];
  ModuleEntry* current_module = &module_map[0];

  while (fgets(line, sizeof(line), file) != NULL) {
    // remove new line character
    for (char& value : line) {
      if (value == '\n') value = '\0';
    }

    // '>' character marks change for submodule
    // '$' character marks an include of a different config file, step into parsing that file
    // all other lines should be forwarded to the module parses, e.g ParseNavigation()
    switch (line[0]) {
      case '#':   // comment
      case '\0':  // empty line
        continue;
      case '>': {
        ModuleEntry* prev_module = current_module;
        for (ModuleEntry& entry : module_map) {
          if (strncmp(entry.name, line+2, BUFFER_SIZE) == 0) {
            current_module = &entry;
            break;
          }
        }

        if (prev_module == current_module) {
          log_.ERR("CONFIG", "module name \"%s\" not found, keeping to module \"%s\""
                  , line+1
                  , current_module->name);
        } else {
          log_.DBG("CONFIG", "changing module to \"%s\"", current_module->name);
        }

        break;
      }
      case '$': {
        // check if config_file already in config_files_
        char* new_config_file = line + 2;
        bool duplicate = false;
        for (char* file : config_files_) {
          if (std::strcmp(file, new_config_file) == 0) {
            duplicate = true;
            break;
          }
        }
        if (duplicate) {
          log_.ERR("CONFIG", "circular config include of %s from %s", new_config_file, config_file);
          break;
        }

        config_files_.push_back(new_config_file);
        log_.DBG("CONFIG", "Stepping into %s, reseting module to \"%s\"",
                            new_config_file, current_module->name);
        current_module = &module_map[0];
        readFile(new_config_file);
        current_module = &module_map[0];
        log_.DBG("CONFIG", "Returning into %s, reseting module to \"%s\"",
                            config_file, current_module->name);
        config_files_.pop_back();
        break;
      }
      default: {
        // dispatch to line parser
        (this->*(current_module->parse)) (line);
      }
    }
  }

  fclose(file);
}

Config::Config(char* config_file)
    : log_(System::getLogger())
{
  config_files_.push_back(config_file);
  readFile(config_file);
  config_files_.pop_back();
}

}}  // namespace hyped::utils
