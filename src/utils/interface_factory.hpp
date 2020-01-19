/*
 * Authors: M. Kristien
 * Organisation: HYPED
 * Date: Dec 2019
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

#ifndef UTILS_INTERFACE_FACTORY_HPP_
#define UTILS_INTERFACE_FACTORY_HPP_
/**
 * @brief An abstract templated factory to provide mapping from string names
 * of abstract interfaces to functions creating the implementations of the interfaces.
 *
 * The factory is templated with the interface class. This means the factory is created
 * individually for different interfaces. Different implementations of the same interface
 * use the same factory.
 */

#include <map>
#include <string>

#include "utils/interfaces.hpp"
#include "utils/logger.hpp"

using std::string;

namespace hyped {
namespace utils {

template<class Interface>
class InterfaceFactory {
  typedef Interface* (*InterfaceCreator)();

 public:
  static InterfaceCreator getCreator(string implementation)
  {
    InterfaceFactory* instance = getInstance();
    auto result = instance->mapping_.find(implementation);
    if (result == instance->mapping_.end()) {
      instance->log_.ERR("FACTORY", "no mapping for interface \"%s\" named \"%s\"",
                                    interfaceName<Interface>(),
                                    implementation.c_str());
      return 0;
    } else {
      return result->second;
    }
  }

  static bool registerCreator(string implementation, InterfaceCreator creator)
  {
    InterfaceFactory* instance = getInstance();
    return instance->mapping_.insert({implementation, creator}).second;
  }

 private:
  // implement singleton
  static InterfaceFactory* getInstance()
  {
    if (!instance_) instance_ = new InterfaceFactory();

    return instance_;
  }
  static InterfaceFactory* instance_;

  InterfaceFactory(): log_(false, -1) {}
  // InterfaceFactory member variables
  Logger log_;
  std::map<string, InterfaceCreator> mapping_;
};

// define static intances (otherwise, linker error)
template <class Interface>
InterfaceFactory<Interface>* InterfaceFactory<Interface>::instance_ = 0;

}}  // namespace hyped::utils

#endif  // UTILS_INTERFACE_FACTORY_HPP_
