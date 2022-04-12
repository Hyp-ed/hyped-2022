#include <debugging/observer.hpp>
#include <debugging/repl.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/system.hpp>

int main(const int argc, const char **argv)
{
  hyped::utils::System::parseArgs(argc, argv);
  auto &sys = hyped::utils::System::getSystem();

  auto observer_optional = hyped::debugging::Observer::fromFile(sys.config_.debugger_config_path);
  if (!observer_optional) { return 1; }
  auto observer = std::move(*observer_optional);
  observer->start();
  hyped::utils::concurrent::Thread::sleep(500);  // provide some initialisation time

  auto repl_optional = hyped::debugging::Repl::fromFile(sys.config_.debugger_config_path);
  if (!repl_optional) { return 2; }
  auto repl = std::move(*repl_optional);
  repl->run();

  observer->join();

  return 0;
}
