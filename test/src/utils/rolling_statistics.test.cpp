
/*
 * Author: Florica Margaritescu
 * Organisation: HYPED
 * Date: 29/10/2020
 * Description: Testing OnlineStatistics class from Statistics.hpp. Tests 
 * functionality and statistical properties of OnlineStatistics class from statistics.hpp.
 *
 *    Copyright 2020 HYPED
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

/**
 * Description:
 * Tests functionality and statistical properties of OnlineStatistics class from statistics.hpp.
 */

#include <math.h>
#include <iostream>
#include <cmath>
#include <numeric>
#include "utils/math/statistics.hpp"
#include "gtest/gtest.h"

namespace hyped {
namespace utils {
namespace math {

/**
* @brief generates a random float between two given floats. 
*/
float RandomFloat(float a, float b) 
{ 
  float random = (static_cast<float>(rand()))/ static_cast<float>(RAND_MAX);
  float diff = b - a;
  float r = random * diff;
  return a + r;
}

/**
* @brief calculates the mean of the elements of a given array
*/
template <class T> 
T meanCalc(T a[], int size_a) 
{
  T sum = 0;
  T mean = 0;
  for (int i =0; i <size_a;i++) {
    sum = sum + a[i];
  }
  mean = (static_cast<double>(sum))/(static_cast<double>((size_a)));
  return mean;
}
 
/**
* @brief Struct used to verify/test integer values/properties for RollingStatistics
*/
struct rollingStatistics_test_int : public ::testing::Test 
{
  protected:
  std::size_t window = 2000;
  RollingStatistics<int> test_stats_int = RollingStatistics<int>(window);
    
  // Declaring variables to be used
  int values_counter = 1000;
  int values[1000];
  int sum = 0;
  int mean = 0;
  int var;
  float c = RandomFloat(1, 1000); 

  void SetUp() 
  {
     for (int i = 0; i < values_counter;i++) {
       values[i] = rand() % 1000 + 1;
      }
  }

  void TearDown() {}
};
     
/**
* @brief Struct used to verify/test float values/properties for RollingStatistics
*/
struct rollingStatistics_test_float : public ::testing::Test 
{
  protected:
  std::size_t window = 2000;
  RollingStatistics<float> test_stats_float = RollingStatistics<float>(window);
    
  // Declaring variables to be used
  int values_counter = 1000;
  float values_f[1000];
  float sum_f = 0.0;
  float mean_f = 0.0;
  float var_f;
  float c = RandomFloat(1, 1000);

  void SetUp() 
  {
    for (int i = 0; i < values_counter;i++) {
      values_f[i] = RandomFloat(1, 1000);
    }
  }
  void TearDown() {}
};

/**
* Tests if the class correctly calculates statistics for one integer variable only. 
* Variance property to be assesed: Var(C) = 0, where C is a constant.
*/
TEST_F(rollingStatistics_test_int, test_default_int_rolling)
{
  test_stats_int.update(values[0]);
  ASSERT_EQ(values[0] , test_stats_int.getMean());
  ASSERT_EQ(0 , test_stats_int.getStdDev());
  ASSERT_EQ(values[0] , test_stats_int.getSum());
  ASSERT_EQ(0 , test_stats_int.getVariance());
}

/**
* Tests if the class correctly calculates statistics for one float variable only. 
* Variance property to be assesed: Var(C) = 0, where C is a constant.
*/
TEST_F(rollingStatistics_test_float, test_default_float_rolling)
{
  test_stats_float.update(values_f[0]);
  ASSERT_EQ(values_f[0], test_stats_float.getMean());
  ASSERT_EQ(0 , test_stats_float.getStdDev());
  ASSERT_EQ(values_f[0] , test_stats_float.getSum());
  ASSERT_EQ(0 , test_stats_float.getVariance());
}

/**
* Tests if the mean and sum are calculated correctly if the class receives an array of n 
* integers (taken one by one - using OnlineStatistics.update(<value>))
*/
TEST_F(rollingStatistics_test_int, test_mean_sum_int_rolling)
{
  for (int i =0; i < values_counter;i++) {
    test_stats_int.update(values[i]);
  }
  mean = meanCalc<int>(values, values_counter);
  sum = std::accumulate(values, values+values_counter, sum);
  ASSERT_EQ(mean , test_stats_int.getMean());
  ASSERT_EQ(sum , test_stats_int.getSum());
}
  
/**
* Tests if the mean and sum are calculated correctly if the class receives an array of n 
* floats (taken one by one - using OnlineStatistics.update(<value>))
*/
TEST_F(rollingStatistics_test_float, test_mean_sum_float_rolling)
{
  for (int i =0; i < values_counter;i++) {
    test_stats_float.update(values_f[i]);
  }
  mean_f = meanCalc<float>(values_f, values_counter);
  sum_f = std::accumulate(values_f, values_f+values_counter, sum_f);
  ASSERT_EQ(mean_f , test_stats_float.getMean());
  ASSERT_EQ(sum_f , test_stats_float.getSum());
}

/**
* Checks if the standard deviance is equal to the root of the variance for an array of n 
* integers (taken one by one - using OnlineStatistics.update(<value>))
*/
TEST_F(rollingStatistics_test_int, test_var_std_int_rolling)
{
  for (int i =0; i < values_counter;i++) {
    test_stats_int.update(values[i]);
    int var_test = static_cast<int>((sqrt(test_stats_int.getVariance())));
    ASSERT_EQ(var_test, test_stats_int.getStdDev());
  }
}

/**
* Checks if the standard deviance is equal to the root of the variance for an array of n 
* floats (taken one by one - using OnlineStatistics.update(<value>))
*/
TEST_F(rollingStatistics_test_float, test_var_std_float_rolling)
{
  for (int i =0; i < values_counter;i++) {
    test_stats_float.update(values_f[i]);
    float var_test_f = static_cast<float>(sqrt(test_stats_float.getVariance()));
    ASSERT_EQ(var_test_f, test_stats_float.getStdDev());
  }
}

/**
* Checks if variance, standard deviance and mean of an integer array is affected by outliers as it 
* should(in this case, outliers will be a random integer from 100 to 200 added onto (3*standard deviance + mean).  
*/
TEST_F(rollingStatistics_test_int, test_int_outliers_rolling)
{
  for (int i =0; i < values_counter;i++) {
    test_stats_int.update(values[i]);
  }
  int var_prev = test_stats_int.getVariance();
  int mean_prev = test_stats_int.getMean();
  int std_dev_prev = test_stats_int.getStdDev();
  int threshold = mean_prev + 3*std_dev_prev;

  for (int i = 0;i < (static_cast<int>(values_counter/10));i++) {
    test_stats_int.update(threshold +  rand() % 200 + 100);
  }
  EXPECT_LT(mean_prev, test_stats_int.getMean());
  EXPECT_LT(var_prev, test_stats_int.getVariance());
  EXPECT_LT(std_dev_prev, test_stats_int.getStdDev());
}

/**
* Checks if variance, standard deviance and mean of an float array is affected by outliers as it
* should(in this case, outliers will be a random float from 100 to 200 added onto (3*standard deviance + mean).  
*/
TEST_F(rollingStatistics_test_float, test_float_outliers_rolling)
{
  for (int i =0; i < values_counter;i++) {
    test_stats_float.update(values_f[i]);
  }
  float var_prev = test_stats_float.getVariance();
  float mean_prev = test_stats_float.getMean();
  float std_dev_prev = test_stats_float.getStdDev();
  float threshold = mean_prev + 3*std_dev_prev;

  for (int i = 0;i < (static_cast<int>(values_counter/10));i++) {
    test_stats_float.update(threshold +  RandomFloat(100, 200));
  }

  EXPECT_LT(mean_prev, test_stats_float.getMean());
  EXPECT_LT(var_prev, test_stats_float.getVariance());
  EXPECT_LT(std_dev_prev, test_stats_float.getStdDev());
}

/**
* Check if statistics update correctly if window size is exceeded (new elments being float outliers 
* if considering statistics for the original set) and last elements are removed from calculations.
*/
TEST_F(rollingStatistics_test_float, test_float_window_rolling)
{
  for (int i = 0; i < values_counter;i++) {
    test_stats_float.update(values_f[i]);
  }
  // Filling up the rest of the window - 2000 items
  for (int i = 0;i < values_counter;i++) {
    test_stats_float.update(RandomFloat(0, 1000));
  }

  float mean_prev = test_stats_float.getMean();
  float sum_prev = test_stats_float.getSum();
  float var_prev = test_stats_float.getVariance();
  float std_dev_prev = test_stats_float.getStdDev();

  for (int i = 0; i <= (static_cast<int>(values_counter/10));i++) {
    // We will purposely assign a larger value for easy comparison
    float new_variable = RandomFloat(2000, 3000); 
    test_stats_float.update(new_variable);
  }

  EXPECT_LT(mean_prev, test_stats_float.getMean());
  EXPECT_LT(sum_prev, test_stats_float.getSum());
  EXPECT_LT(var_prev, test_stats_float.getVariance());
  EXPECT_LT(std_dev_prev, test_stats_float.getStdDev());
}

/**
 * Check if statistics update correctly if window size is exceeded (new elments 
 * being integer outliers if considering statistics for the original set) and last 
 * elements are removed from calculations.
 */

TEST_F(rollingStatistics_test_int, test_int_window_rolling)
{
  for (int i = 0; i < values_counter;i++) {
    test_stats_int.update(values[i]);
  }

  // Filling up the rest of the window - 2000 items
  for (int i = 0;i < values_counter;i++) {
    test_stats_int.update(rand() % 1000 + 1);
  }

  int mean_prev = test_stats_int.getMean();
  int sum_prev = test_stats_int.getSum();
  int var_prev = test_stats_int.getVariance();
  int std_dev_prev = test_stats_int.getStdDev();

  for (int i = 0; i <= (static_cast<int>(values_counter/10));i++) {
    // We will purposely assign a larger value for easy comparison
    int new_variable = rand() % 3000 + 2000; 
    test_stats_int.update(new_variable);
  }

  EXPECT_LT(mean_prev, test_stats_int.getMean());
  EXPECT_LT(sum_prev, test_stats_int.getSum());
  EXPECT_LT(var_prev, test_stats_int.getVariance());
  EXPECT_LT(std_dev_prev, test_stats_int.getStdDev());
}
}}}  // hyped::utils::math
