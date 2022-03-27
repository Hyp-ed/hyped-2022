#include <cmath>
#include <math.h>

#include <iostream>
#include <numeric>
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
 * @brief generates a random float between two given floats. Code from: https://tinyurl.com/y2phu2q2
 * @tparam T Underlying numeric type
 * @param lower Lower bound for randomly generated values
 * @param upper Upper bound for randomly generated values
 */
float RandomFloatRolling(float a, float b)
{
  float random = (static_cast<float>(rand())) / RAND_MAX;
  float diff   = b - a;
  float r      = random * diff;
  return a + r;
}

/**
 * @brief calculates the mean of the elements of a given array
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
 * @brief class used to verify/test integer values/properties for RollingStatistics
 */
class RollingStatisticsTestInt : public ::testing::Test {
 protected:
  std::size_t window                    = 2000;
  RollingStatistics<int> test_stats_int = RollingStatistics<int>(window);

  // Declaring variables to be used
  static constexpr size_t kNumValues = 1000;
  int values[1000];
  int sum  = 0;
  int mean = 0;
  int var;
  float c                        = RandomFloatRolling(1, 1000);
  std::string messageVarStdDev   = "Standard deviation is not the root of the variance as expected";
  std::string messageOutlierMean = "Outliers do not affect the mean as expected.";
  std::string messageOutlierVar  = "Outliers do not affect the variance as expected.";
  std::string messageOutlierStdDev = "Outliers do not affect the standard deviation as expected.";

  void SetUp()
  {
    for (size_t i = 0; i < kNumValues; ++i) {
      values[i] = rand() % 1000 + 1;
    }
  }

  void TearDown() {}
};

/**
* @brief tests if the class calculates statistics coerrectly for one integer variable
only. Variance property to be assesed: Var(C) = 0, where C is a constant.
*/
TEST_F(RollingStatisticsTestInt, testDefaultIntRolling)
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
TEST_F(RollingStatisticsTestInt, testMeanSumIntRolling)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_int.update(values[i]);
  }
  mean = meanCalc<int>(values, kNumValues);
  sum  = std::accumulate(values, values + kNumValues, sum);
  ASSERT_EQ(mean, test_stats_int.getMean());
  ASSERT_EQ(sum, test_stats_int.getSum());
}

/**
 * @brief checks if the standard deviation is equal to the root of the variance for an array of n
 * integers.
 */
TEST_F(RollingStatisticsTestInt, testVarStdIntRolling)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_int.update(values[i]);
    int var_test = static_cast<int>((sqrt(test_stats_int.getVariance())));
    ASSERT_EQ(var_test, test_stats_int.getStdDev()) << messageVarStdDev;
  }
}

/**
 * @brief checks if variance, standard deviation and mean of an integer array are affected by
 * outliers as they should(in this case, outliers will be random integers from 100 to 200 added onto
 * (3*standard deviation + mean).
 */
TEST_F(RollingStatisticsTestInt, testOutlierrsIntRolling)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_int.update(values[i]);
  }
  int var_prev     = test_stats_int.getVariance();
  int mean_prev    = test_stats_int.getMean();
  int std_dev_prev = test_stats_int.getStdDev();
  int threshold    = mean_prev + 3 * std_dev_prev;

  for (size_t i = 0; i < (static_cast<int>(kNumValues / 10)); ++i) {
    test_stats_int.update(threshold + rand() % 200 + 100);
  }
  EXPECT_LT(mean_prev, test_stats_int.getMean()) << messageOutlierMean;
  EXPECT_LT(var_prev, test_stats_int.getVariance()) << messageOutlierVar;
  EXPECT_LT(std_dev_prev, test_stats_int.getStdDev()) << messageOutlierStdDev;
}

/**
 * @brief check if statistics update correctly if window size is exceeded (using outliers for easy
 * comparison) and last elements are removed from calculations.
 */

TEST_F(RollingStatisticsTestInt, testWindowIntRolling)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_int.update(values[i]);
  }

  // Filling up the rest of the window - 2000 items
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_int.update(rand() % 1000 + 1);
  }

  int mean_prev    = test_stats_int.getMean();
  int sum_prev     = test_stats_int.getSum();
  int var_prev     = test_stats_int.getVariance();
  int std_dev_prev = test_stats_int.getStdDev();

  for (size_t i = 0; i <= kNumValues / 10; ++i) {
    // We will purposely assign a larger value for easy comparison
    int new_variable = rand() % 3000 + 2000;
    test_stats_int.update(new_variable);
  }

  EXPECT_LT(mean_prev, test_stats_int.getMean());
  EXPECT_LT(sum_prev, test_stats_int.getSum());
  EXPECT_LT(var_prev, test_stats_int.getVariance());
  EXPECT_LT(std_dev_prev, test_stats_int.getStdDev());
}

// -------------------------------------------------------------------------------------------------
// Property and Functionality Tests for Float Values
// -------------------------------------------------------------------------------------------------

/**
 * @brief class used to verify/test float values/properties for RollingStatistics
 */
class RollingStatisticsTestFloat : public ::testing::Test {
 protected:
  std::size_t window                        = 2000;
  RollingStatistics<float> test_stats_float = RollingStatistics<float>(window);

  // Declaring variables to be used
  static constexpr size_t kNumValues = 1000;
  float values_f[kNumValues];
  float sum_f  = 0.0;
  float mean_f = 0.0;
  float var_f;
  float c                        = RandomFloatRolling(1, 1000);
  std::string messageVarStdDev   = "Standard deviation is not the root of the variance as expected";
  std::string messageOutlierMean = "Outliers do not affect the mean as expected.";
  std::string messageOutlierVar  = "Outliers do not affect the variance as expected.";
  std::string messageOutlierStdDev = "Outliers do not affect the standard deviation as expected.";

  void SetUp()
  {
    for (size_t i = 0; i < kNumValues; ++i) {
      values_f[i] = RandomFloatRolling(1, 1000);
    }
  }
  void TearDown() {}
};

/**
 * @brief tests if the class calculates statistics correctly for one float variable only.
 * Variance property to be assesed: Var(C) = 0, where C is a constant.
 */
TEST_F(RollingStatisticsTestFloat, testDefaultFloatRolling)
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
TEST_F(RollingStatisticsTestFloat, testMeanSumFloatRolling)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_float.update(values_f[i]);
  }
  mean_f = meanCalc<float>(values_f, kNumValues);
  sum_f  = std::accumulate(values_f, values_f + kNumValues, sum_f);
  ASSERT_EQ(mean_f, test_stats_float.getMean());
  ASSERT_EQ(sum_f, test_stats_float.getSum());
}

/**
 * @brief checks if the standard deviation is equal to the root of the variance for an array of n
 * floats.
 */
TEST_F(RollingStatisticsTestFloat, testVarStdFloatRolling)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_float.update(values_f[i]);
    float var_test_f = sqrt(test_stats_float.getVariance());
    ASSERT_EQ(var_test_f, test_stats_float.getStdDev()) << messageVarStdDev;
  }
}

/**
 * @brief checks if variance, standard deviation and mean of an float array are affected by outliers
 * as they should(in this case, outliers will be random floats from 100 to 200 added onto
 * (3*standard deviation + mean).
 */
TEST_F(RollingStatisticsTestFloat, testOutliersFloatRolling)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_float.update(values_f[i]);
  }
  float var_prev     = test_stats_float.getVariance();
  float mean_prev    = test_stats_float.getMean();
  float std_dev_prev = test_stats_float.getStdDev();
  float threshold    = mean_prev + 3 * std_dev_prev;

  for (size_t i = 0; i < (static_cast<size_t>(kNumValues / 10)); ++i) {
    test_stats_float.update(threshold + RandomFloatRolling(100, 200));
  }

  EXPECT_LT(mean_prev, test_stats_float.getMean()) << messageOutlierMean;
  EXPECT_LT(var_prev, test_stats_float.getVariance()) << messageOutlierVar;
  EXPECT_LT(std_dev_prev, test_stats_float.getStdDev())
    << "Outliers do not affect the standard deviation as expected.";
}

/**
 * @brief checks if statistics update correctly if window size is exceeded (adding outliers for
 * easy comparison) and last elements are removed from calculations.
 */
TEST_F(RollingStatisticsTestFloat, testWindowsFloatRolling)
{
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_float.update(values_f[i]);
  }
  // Filling up the rest of the window - 2000 items
  for (size_t i = 0; i < kNumValues; ++i) {
    test_stats_float.update(RandomFloatRolling(0, 1000));
  }

  float mean_prev    = test_stats_float.getMean();
  float sum_prev     = test_stats_float.getSum();
  float var_prev     = test_stats_float.getVariance();
  float std_dev_prev = test_stats_float.getStdDev();

  for (size_t i = 0; i <= kNumValues / 10; ++i) {
    // We will purposely assign a larger value for easy comparison
    float new_variable = RandomFloatRolling(2000, 3000);
    test_stats_float.update(new_variable);
  }

  EXPECT_LT(mean_prev, test_stats_float.getMean());
  EXPECT_LT(sum_prev, test_stats_float.getSum());
  EXPECT_LT(var_prev, test_stats_float.getVariance());
  EXPECT_LT(std_dev_prev, test_stats_float.getStdDev());
}

}  // namespace math
}  // namespace utils
}  // namespace hyped
