#include "randomiser.hpp"

#include <cmath>
#include <math.h>

#include <iostream>
#include <numeric>
#include <random>
#include <string>

#include <gtest/gtest.h>

#include <utils/math/statistics.hpp>

namespace hyped {
namespace utils {
namespace math {

// -------------------------------------------------------------------------------------------------
// Helper Functions
// -------------------------------------------------------------------------------------------------

/**
 * @brief calculates the mean of the elements of a given array
 * @tparam T Underlying numeric type
 * @param a Array that we will get summation items from
 * @param size_a Size of said array
 */
template<class T>
T meanCalc(const T a[], const size_t size_a)
{
  T sum  = 0;
  T mean = 0;
  for (size_t i = 0; i < size_a; ++i) {
    sum = sum + a[i];
  }
  mean = sum / size_a;
  return mean;
}

// -------------------------------------------------------------------------------------------------
// Property and Functionality Tests for Integer Values
// -------------------------------------------------------------------------------------------------

/**
 * @brief class used for test fixtures checking class functionality and properties for integer
 * values.
 */
class OnlineStatisticsTestInt : public ::testing::Test {
 protected:
  hyped::utils::math::OnlineStatistics<int> test_stats_int;
  static constexpr size_t kNumValues = 1000;
  int values[kNumValues];
  int sum  = 0;
  int mean = 0;
  int var;
  float c                        = testing::Randomiser::randomInRange(1, 1000);
  std::string messageVarStdDev   = "Standard deviation is not the root of the variance as expected";
  std::string messageOutlierMean = "Outliers do not affect the mean as expected.";
  std::string messageOutlierVar  = "Outliers do not affect the variance as expected.";
  std::string messageOutlierStdDev = "Outliers do not affect the standard deviation as expected.";

  void SetUp()
  {
    // Populating the array to be used with random integers
    for (size_t i = 0; i < kNumValues; ++i) {
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
  ASSERT_EQ(values[0], test_stats_int.getMean());
  ASSERT_EQ(0, test_stats_int.getStdDev());
  ASSERT_EQ(values[0], test_stats_int.getSum());
  ASSERT_EQ(0, test_stats_int.getVariance());
}

/**
 * @brief tests if the mean and sum are calculated correctly if the class receives an array of n
 * integers.
 */
TEST_F(OnlineStatisticsTestInt, testMeanSumIntOnline)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_int.update(values[i]);
  }
  mean = meanCalc<int>(values, kNumValues);
  sum  = std::accumulate(values, values + kNumValues, sum);
  ASSERT_EQ(mean, test_stats_int.getMean());
  const auto sum_test = test_stats_int.getSum();
  ASSERT_EQ(sum_test / static_cast<ssize_t>(kNumValues), test_stats_int.getMean());
  ASSERT_EQ(sum, test_stats_int.getSum());
}

/**
 * @brief checks if the standard deviation is equal to the root of the variance for an
 * array of n integers.
 */
TEST_F(OnlineStatisticsTestInt, testVarStdIntOnline)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_int.update(values[i]);
    int var_test = static_cast<int>(sqrt(test_stats_int.getVariance()));
    ASSERT_EQ(var_test, test_stats_int.getStdDev()) << messageVarStdDev;
  }
}

/**
 * @brief checks if variance, standard deviation and mean of an integer array are
 * affected by outliers as they should(in this case, outliers will be random
 * integers from 100 to 200 added onto (3*standard deviation + mean).
 */
TEST_F(OnlineStatisticsTestInt, testOutliersIntOnline)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_int.update(values[i]);
  }
  int var_prev     = test_stats_int.getVariance();
  int mean_prev    = test_stats_int.getMean();
  int std_dev_prev = test_stats_int.getStdDev();
  int threshold    = mean_prev + 3 * std_dev_prev;

  // Introducing the outliers in our data
  for (size_t i = 0; i < (static_cast<int>(kNumValues) / 10); ++i) {
    test_stats_int.update(threshold + rand() % 200 + 100);
  }
  EXPECT_LT(mean_prev, test_stats_int.getMean()) << messageOutlierMean;
  EXPECT_LT(var_prev, test_stats_int.getVariance()) << messageOutlierVar;
  EXPECT_LT(std_dev_prev, test_stats_int.getStdDev()) << messageOutlierStdDev;
}

// -------------------------------------------------------------------------------------------------
// Property and Functionality Tests for Float Values
// -------------------------------------------------------------------------------------------------

/**
 * @brief class used for test fixtures checking class functionality and properties for float
 * values.
 */
class OnlineStatisticsTestFloat : public ::testing::Test {
 protected:
  hyped::utils::math::OnlineStatistics<float> test_stats_float;
  // Declaring variables to be used
  static constexpr size_t kNumValues = 1000;
  float values_f[kNumValues];
  float sum_f  = 0.0;
  float mean_f = 0.0;
  float var_f;
  float c                        = testing::Randomiser::randomInRange(1, 1000);
  std::string messageVarStdDev   = "Standard deviation is not the root of the variance as expected";
  std::string messageOutlierMean = "Outliers do not affect the mean as expected.";
  std::string messageOutlierVar  = "Outliers do not affect the variance as expected.";
  std::string messageOutlierStdDev = "Outliers do not affect the standard deviation as expected.";

  void SetUp()
  {
    // Populating the array to be used with random floats
    for (size_t i = 0; i < kNumValues; ++i) {
      values_f[i] = testing::Randomiser::randomInRange(1, 1000);
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
  ASSERT_EQ(0, test_stats_float.getStdDev());
  ASSERT_EQ(values_f[0], test_stats_float.getSum());
  ASSERT_EQ(0, test_stats_float.getVariance());
}

/**
 * @brief tests if the mean and sum are calculated correctly if the class receives an array of n
 * floats.
 */
TEST_F(OnlineStatisticsTestFloat, testMeanSumFloatOnline)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_float.update(values_f[i]);
  }
  mean_f = meanCalc<float>(values_f, kNumValues);
  sum_f  = std::accumulate(values_f, values_f + kNumValues, sum_f);
  ASSERT_EQ(mean_f, test_stats_float.getMean());
  float sum_f_test = (static_cast<float>(test_stats_float.getSum()));
  ASSERT_EQ(sum_f_test / kNumValues, test_stats_float.getMean());
  ASSERT_EQ(sum_f, test_stats_float.getSum());
}

/**
 * @brief checks if the standard deviation is equal to the root of the variance for
 * an array of n floats.
 */
TEST_F(OnlineStatisticsTestFloat, testVarStdFloatOnline)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_float.update(values_f[i]);
    float var_f_test = sqrt(test_stats_float.getVariance());
    ASSERT_EQ(var_f_test, test_stats_float.getStdDev()) << messageVarStdDev;
  }
}

/**
 * @brief checks if variance, standard deviation and mean of an float array are affected by outliers
 * as they should(in this case, outliers will be random floats from 100 to 200 added onto
 * (3*standard deviation + mean).
 */
TEST_F(OnlineStatisticsTestFloat, testOutliersFloatOnline)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_float.update(values_f[i]);
  }

  float var_prev     = test_stats_float.getVariance();
  float mean_prev    = test_stats_float.getMean();
  float std_dev_prev = test_stats_float.getStdDev();
  float threshold    = mean_prev + 3 * std_dev_prev;

  //  Introducing the outliers in our data
  for (size_t i = 0; i < (static_cast<int>(kNumValues) / 10); ++i) {
    test_stats_float.update(threshold + testing::Randomiser::randomInRange(100, 200));
  }

  EXPECT_LT(mean_prev, test_stats_float.getMean()) << messageOutlierMean;
  EXPECT_LT(var_prev, test_stats_float.getVariance()) << messageOutlierVar;
  EXPECT_LT(std_dev_prev, test_stats_float.getStdDev()) << messageOutlierStdDev;
}
}  // namespace math
}  // namespace utils
}  // namespace hyped
