#include <debugging/repl.hpp>
#include <utils/system.hpp>

int main(const int argc, const char **argv)
{
  hyped::utils::System::parseArgs(argc, argv);
  auto &sys = hyped::utils::System::getSystem();
  auto repl = hyped::debugging::Repl::fromFile(sys.config_.debugger_config_path);
  if (repl) { repl->run(); }
  return 0;
}
