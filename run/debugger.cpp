#include <debugging/repl.hpp>

int main(const int argc, const char **argv)
{
  auto repl = hyped::debugging::Repl::fromArgs(argc, argv);
  if (repl) { repl->run(); }
  return 0;
}
