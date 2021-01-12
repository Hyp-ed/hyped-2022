
/*
 * Author: Florica Margaritescu
 * Organisation: HYPED
 * Date: 29/10/2020
 * Description: Tests functionality and statistical properties of OnlineStatistics class from statistics.hpp.
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

#include <math.h>
#include <iostream>
#include <string>
#include <cmath>
#include <numeric>
#include "utils/math/statistics.hpp"
#include "gtest/gtest.h"

namespace hyped {
namespace utils {
namespace math {

// -------------------------------------------------------------------------------------------------
// Helper Functions
// -------------------------------------------------------------------------------------------------

/**
* @brief generates a random float between two given floats. Code from: https://tinyurl.com/y2phu2q2
* @tparam T Underlying numeric type
* @param lower Lower bound for randomly generated values
* @param upper Upper bound for randomly generated values
*/
float RandomFloatOnline(float lower, float upper)
{
  float random = (static_cast<float>(rand())) / RAND_MAX;
  float diff = upper - lower;
  float r = random * diff;
  return lower + r;
}

/**
* @brief calculates the mean of the elements of a given array
* @tparam T Underlying numeric type
* @param a Array that we will get summation items from
* @param size_a Size of said array
*/
template <class T> 
T meanCalc(T a[], int size_a)
{
  T sum = 0;
  T mean = 0;
  for (int i = 0; i <size_a;i++) {
    sum = sum + a[i];
  }
  mean = sum/size_a;
  return mean;
}

// -------------------------------------------------------------------------------------------------
// Property and Functionality Tests for Integer Values
// -------------------------------------------------------------------------------------------------

/**
* @brief struct used for test fixtures checking class functionality and properties for integer values.
*/
struct OnlineStatisticsTestInt : public ::testing::Test 
{
  protected:
    hyped::utils::math::OnlineStatistics<int> test_stats_int;
    int values_counter = 1000;
    int values[1000];
    int sum = 0;
    int mean = 0;
    int var;
    float c = RandomFloatOnline(1, 1000);
    std::string messageVarStdDev = 
       "Standard deviation is not the root of the variance as expected";
    std::string messageOutlierMean = 
       "Outliers do not affect the mean as expected.";
    std::string messageOutlierVar = 
       "Outliers do not affect the variance as expected.";
    std::string messageOutlierStdDev = 
       "Outliers do not affect the standard deviation as expected.";


  void SetUp()
  {
    // Populating the array to be used with random integers
    for (int i = 0; i < values_counter;i++) {
      values[i] = rand() % 1000 + 1; 
    }
  }
  void TearDown() {}
};

/**
* @brief tests if the class calculates statistics correctly for one integer variable only. 
* Variance property to be assesed: Var(C) = 0, where C is a constant.
*/

TEST_F(OnlineStatisticsTestInt, testDefaultIntOnline)
{
  test_stats_int.update(values[0]);
  ASSERT_EQ(values[0] , test_stats_int.getMean());
  ASSERT_EQ(0 , test_stats_int.getStdDev());
  ASSERT_EQ(values[0] , test_stats_int.getSum());
  ASSERT_EQ(0 , test_stats_int.getVariance());
}

/**
* @brief tests if the mean and sum are calculated correctly if the class receives an array of n 
* integers.
*/
TEST_F(OnlineStatisticsTestInt, testMeanSumIntOnline) 
{
  for (int i = 0; i < values_counter;i++) {
    test_stats_int.update(values[i]);
  }
  mean = meanCalc<int>(values, values_counter);
  sum = std::accumulate(values, values+values_counter, sum);
  ASSERT_EQ(mean, test_stats_int.getMean());
  int sum_test = static_cast<int>(test_stats_int.getSum());
  ASSERT_EQ(sum_test/values_counter , test_stats_int.getMean());
  ASSERT_EQ(sum, test_stats_int.getSum());
}
    
/**
* @brief checks if the standard deviation is equal to the root of the variance for an
* array of n integers.
*/
TEST_F(OnlineStatisticsTestInt, testVarStdIntOnline) 
{
  for (int i = 0; i < values_counter; i++) {
    test_stats_int.update(values[i]);
  int var_test = static_cast<int>(sqrt(test_stats_int.getVariance()));
  ASSERT_EQ(var_test, test_stats_int.getStdDev())
      << messageVarStdDev;
  }
}

/**
* @brief checks if variance, standard deviation and mean of an integer array are
* affected by outliers as they should(in this case, outliers will be random
* integers from 100 to 200 added onto (3*standard deviation + mean).  
*/
TEST_F(OnlineStatisticsTestInt, testOutliersIntOnline) 
{
  for (int i = 0; i < values_counter; i++) {
    test_stats_int.update(values[i]);
  }
  int var_prev = test_stats_int.getVariance();
  int mean_prev = test_stats_int.getMean();
  int std_dev_prev = test_stats_int.getStdDev();
  int threshold = mean_prev + 3*std_dev_prev;

  // Introducing the outliers in our data
  for (int i = 0;i < (static_cast<int>(values_counter)/10);i++) {
    test_stats_int.update(threshold +  rand() % 200 + 100);
  }
  EXPECT_LT(mean_prev, test_stats_int.getMean())
      << messageOutlierMean;
  EXPECT_LT(var_prev, test_stats_int.getVariance())
      << messageOutlierVar;
  EXPECT_LT(std_dev_prev, test_stats_int.getStdDev())
      << messageOutlierStdDev;
}

// -------------------------------------------------------------------------------------------------
// Property and Functionality Tests for Float Values
// -------------------------------------------------------------------------------------------------

/**
* @brief struct used for test fixtures checking class functionality and properties for float values.
*/
struct OnlineStatisticsTestFloat : public ::testing::Test 
{
    protected:
      hyped::utils::math::OnlineStatistics<float> test_stats_float;
      // Declaring variables to be used
      int values_counter = 1000;
      float values_f[1000];
      float sum_f = 0.0;
      float mean_f = 0.0;
      float var_f;
      float c = RandomFloatOnline(1, 1000);
      std::string messageVarStdDev = 
         "Standard deviation is not the root of the variance as expected";
      std::string messageOutlierMean = 
         "Outliers do not affect the mean as expected.";
      std::string messageOutlierVar = 
         "Outliers do not affect the variance as expected.";
      std::string messageOutlierStdDev = 
         "Outliers do not affect the standard deviation as expected.";


    void SetUp()
    {
      // Populating the array to be used with random floats
       for (int i = 0; i < values_counter;i++) {
           values_f[i] = RandomFloatOnline(1, 1000);
        }
    }
   
    void TearDown() {}
};

/**
* @brief tests if the class calculates statistics correctly for one float variable only. 
* Variance property to be assesed: Var(C) = 0, where C is a constant.
*/

TEST_F(OnlineStatisticsTestFloat, testDefaultFloatOnline)
{
  test_stats_float.update(values_f[0]);
  ASSERT_EQ(values_f[0], test_stats_float.getMean());
  ASSERT_EQ(0 , test_stats_float.getStdDev());
  ASSERT_EQ(values_f[0] , test_stats_float.getSum());
  ASSERT_EQ(0 , test_stats_float.getVariance());
}

/**
* @brief tests if the mean and sum are calculated correctly if the class receives an array of n 
* floats.
*/
TEST_F(OnlineStatisticsTestFloat, testMeanSumFloatOnline) 
{
  for (int i = 0; i < values_counter; i++) {
    test_stats_float.update(values_f[i]);
  }
  mean_f = meanCalc<float>(values_f, values_counter);
  sum_f = std::accumulate(values_f, values_f+values_counter, sum_f);
  ASSERT_EQ(mean_f, test_stats_float.getMean());
  float sum_f_test = (static_cast<float>(test_stats_float.getSum()));
  ASSERT_EQ(sum_f_test/values_counter, test_stats_float.getMean());
  ASSERT_EQ(sum_f, test_stats_float.getSum());
}
    
/**
* @brief checks if the standard deviation is equal to the root of the variance for
* an array of n floats.
*/
TEST_F(OnlineStatisticsTestFloat, testVarStdFloatOnline) 
{
  for (int i = 0; i < values_counter; i++) {
    test_stats_float.update(values_f[i]);
    float var_f_test = sqrt(test_stats_float.getVariance());
    ASSERT_EQ(var_f_test, test_stats_float.getStdDev()) 
       << messageVarStdDev;
  }
}

/**
* @brief checks if variance, standard deviation and mean of an float array are affected by outliers as they
* should(in this case, outliers will be random floats from 100 to 200 added onto (3*standard deviation + mean).  
*/
TEST_F(OnlineStatisticsTestFloat, testOutliersFloatOnline) 
{
  for (int i = 0; i < values_counter;i++) {
    test_stats_float.update(values_f[i]);
  }

  float var_prev = test_stats_float.getVariance();
  float mean_prev = test_stats_float.getMean();
  float std_dev_prev = test_stats_float.getStdDev();
  float threshold = mean_prev + 3*std_dev_prev;

  //  Introducing the outliers in our data
  for (int i = 0;i < (static_cast<int>(values_counter)/10);i++) {
    test_stats_float.update(threshold +  RandomFloatOnline(100, 200));
  }
        
  EXPECT_LT(mean_prev, test_stats_float.getMean()) 
     << messageOutlierMean;
  EXPECT_LT(var_prev, test_stats_float.getVariance()) 
     << messageOutlierVar;
  EXPECT_LT(std_dev_prev, test_stats_float.getStdDev())
     << messageOutlierStdDev;
}
    }}}  // hyped::utils::math
