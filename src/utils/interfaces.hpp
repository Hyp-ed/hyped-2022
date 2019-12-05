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

#ifndef UTILS_INTERFACES_HPP_
#define UTILS_INTERFACES_HPP_

/**
 * @brief This header contains a list of all interfaces that can be use to abstract
 * simulated drivers. Only interfaces listed here can be configured through config.txt
 * Factory module.
 *
 * Adding new interfaces is as easy as extending the interface list below. Interfaces must be
 * specified in (almost) full namespace (hyped:: can be omitted as it is implied). This is done
 * by listing both the interface's namespace and the interface class itself.
 *
 * NOTE: Interface class must be unique as the interfaces are resolved only based on the
 * class name, not the full namespaced name.
 */

// use format V(module/namespace, class)
// e.g.       V(utils::config::fancy::typed::here, MyFancyInterface)
#define INTERFACE_LIST(V) \
  V(sensors, ImuInterface) \
  V(demo, DemoInterface)


////////////////////////////////////////////////////////////////////////////////////////////////////
// Below is some supporting code, no need to touch this when adding new interfaces
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace hyped {

// Forward declare all known interfaces so that they can be used for template specialization.
#define FORWARD_DECLARE(module, interface) \
  namespace module { class interface; }
INTERFACE_LIST(FORWARD_DECLARE)

// Declare template function to convert interface type to string.
// Without template specialization for a particular template parameter, the compilation
// should fail during linking.
template<class T>
constexpr const char* interfaceName();

// Specialize above template function to provide the string values for known interfaces.
#define AS_STRING(module, interface)  \
  template<>                          \
  constexpr const char* interfaceName<module::interface>() {return #interface;}
INTERFACE_LIST(AS_STRING)

}   // namespace hyped

#endif  // UTILS_INTERFACES_HPP_
