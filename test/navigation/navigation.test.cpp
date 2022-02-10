#include "randomiser.hpp"
#include "test.hpp"

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include <data/data.hpp>
#include <navigation/navigation.hpp>
#include <utils/logger.hpp>

struct NavigationTest : public hyped::testing::Test {
 protected:
  // Run before each test
  void SetUp() {}
  void TearDown() {}
};

namespace hyped::navigation {
TEST_F(NavigationTest, correctEmergencyBrakingDistance)
{
  Data &data = Data::getInstance();
  Sensors sensors_data;
  hyped::navigation::Navigation nav(log_, 0);

  // unsure of how or what to set the sensors data to
  Randomiser::randomiseSensorsData(sensors_data);

  data.setSensorsData(sensors_data);

  // this should read the sensors data and use it to update velocity etc. but doesn't seem to
  nav.navigate();
  std::cout << data.getNavigationData().velocity;
}
}  // namespace hyped::navigation