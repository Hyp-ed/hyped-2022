#include <iostream>

#include <debugging/observer.hpp>
#include <debugging/repl.hpp>
#include <utils/system.hpp>

int main(const int argc, const char **argv)
{
  hyped::utils::System::parseArgs(argc, argv);
  auto &sys = hyped::utils::System::getSystem();
  std::cout << "Initialising observer" << std::endl;
  auto observer_optional = hyped::debugging::Observer::fromFile(sys.config_.debugger_config_path);
  if (!observer_optional) { return 1; }
  auto observer = std::move(*observer_optional);
  std::cout << "Initialising REPL" << std::endl;
  auto repl = hyped::debugging::Repl::fromFile(sys.config_.debugger_config_path);
  observer->start();
  observer->join();
  return 0;
}
