#pragma once

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

#include "interfaces.hpp"
#include "logger.hpp"

using std::string;

namespace hyped {
namespace utils {

template<class Interface>
class InterfaceFactory {
  typedef Interface *(*InterfaceCreator)();

 public:
  static InterfaceCreator getCreator(string implementation)
  {
    InterfaceFactory *instance = getInstance();
    auto result                = instance->mapping_.find(implementation);
    if (result == instance->mapping_.end()) {
      instance->log_.ERR("FACTORY", "no mapping for interface \"%s\" named \"%s\"",
                         interfaceName<Interface>(), implementation.c_str());
      return 0;
    } else {
      return result->second;
    }
  }

  static bool registerCreator(string implementation, InterfaceCreator creator)
  {
    InterfaceFactory *instance = getInstance();
    return instance->mapping_.insert({implementation, creator}).second;
  }

 private:
  // implement singleton
  static InterfaceFactory *getInstance()
  {
    if (!instance_) instance_ = new InterfaceFactory();

    return instance_;
  }
  static InterfaceFactory *instance_;

  InterfaceFactory() : log_(false, -1) {}
  // InterfaceFactory member variables
  Logger log_;
  std::map<string, InterfaceCreator> mapping_;
};

// define static intances (otherwise, linker error)
template<class Interface>
InterfaceFactory<Interface> *InterfaceFactory<Interface>::instance_ = 0;

}  // namespace utils
}  // namespace hyped
