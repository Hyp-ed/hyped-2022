#include <iostream>

#include <gtest/gtest.h>

#include <utils/system.hpp>

int main(int argc, char **argv)
{
  // Initialising single system instance to be used across all tests
  // construct argument list to pass to System::parseArgs
  static char arg0[] = "testing.cpp";
  static char arg1[] = "--config=test/config.txt";
  char *args[]       = {arg0, arg1};
  hyped::utils::System::parseArgs(2, args);

  std::cout << "running\n";
  ::testing::InitGoogleTest(&argc, argv);

  return (RUN_ALL_TESTS());
}
