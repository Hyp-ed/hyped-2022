#include <iostream>

#include <gtest/gtest.h>

#include <utils/system.hpp>

int main(int argc, char **argv)
{
  std::cout << "running\n";
  ::testing::InitGoogleTest(&argc, argv);

  return (RUN_ALL_TESTS());
}
