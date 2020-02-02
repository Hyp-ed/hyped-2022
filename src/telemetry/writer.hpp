/*
 * Author: Robertas Norkus
 * Organisation: HYPED
 * Date: January 2020
 * Description:
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

#ifndef TELEMETRY_WRITER_HPP_
#define TELEMETRY_WRITER_HPP_

#include <rapidjson/writer.h>
#include <string>
#include "data/data.hpp"

namespace hyped {
namespace telemetry {

  class Writer {
    public:
      explicit Writer(data::Data& data);
      void start();
      void end();
      void packCrucialData();
      void packStatusData();
      void packAdditionalData();
      std::string getString();

    private:
      rapidjson::Writer<rapidjson::StringBuffer> rjwriter_;
      rapidjson::StringBuffer sb_;
      void add(std::string name, int min, int max, std::string unit, int value);
      void add(std::string name, float min, float max, std::string unit, float value);
      void add(std::string name, bool value);
      void add(std::string name, std::string value);
      void add(std::string name, data::State value);
      void add(std::string name, data::ModuleStatus value);
      void startList(std::string name);
      void endList();
      std::string convertStateMachineState(data::State state);
      std::string convertModuleStatus(data::ModuleStatus module_status);
      data::Data& data_;
  };

}  // namespace telemetry
}  // namespace hyped

#endif  // TELEMETRY_WRITER_HPP_
