/*
 * Author: Donald Jennings
 * Organisation: HYPED
 * Date: 21/10/2020
 * Description: Testing file for Differentiator.hpp
 *
 *    Copyright 2018 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "gtest/gtest.h"
#include "utils/math/differentiator.hpp"
#include "utils/system.hpp"
#include "data/data_point.hpp"

using hyped::data::DataPoint;
using hyped::utils::math::Differentiator;
// Used for Test Fixture
struct differentiator_test : public ::testing::Test {
  protected:
    Differentiator<float> diff_test;
    DataPoint<float> test_point;
    DataPoint<float> second_point;
        
    void SetUp() 
    {
      // Sets values of Data points
      test_point.value = 10.5;
      test_point.timestamp = 1;

      second_point.value = 18.6;
      second_point.timestamp = 2;
    }
        
    /**
     * @brief Returns the difference between two data point values.
    */
    float deltaValue(DataPoint<float> point_1, DataPoint<float> prev_point) 
    {
      return point_1.value - prev_point.value;
    }
        
    /**
     * @brief Returns the difference between two data point timestamps
    */
    float deltaTime(DataPoint<float> point_1, DataPoint<float> prev_point) 
    {
      return point_1.timestamp- prev_point.timestamp;
    }

    /**
     * @brief Returns the gradient between two data points
    */
    float gradientOfPoints(DataPoint<float> point_1, DataPoint<float> prev_point) 
    {
      return (deltaValue(point_1, prev_point))/((deltaTime(point_1, prev_point))/1e6);
    }
    void TearDown() {}
};

// Test fixture for determining whether initalisation works correctly
TEST_F(differentiator_test, DifferentiatorInitialised)
{ 
  ASSERT_EQ(diff_test.update(test_point).value, 0);
};

// Test fixture for ensuring gradient is correct
TEST_F(differentiator_test, DifferentiatorGradient)
{   
  diff_test.update(test_point);
  ASSERT_FLOAT_EQ(diff_test.update(second_point).value, gradientOfPoints(second_point, test_point));
}

// Test fixture for ensuring timestamp remains unchanged.
TEST_F(differentiator_test, DifferentiatorTimestampReturn)  
{
  diff_test.update(test_point);
  ASSERT_EQ(diff_test.update(second_point).timestamp, second_point.timestamp);
}
