/*
 * Author: Donald Jennings
 * Organisation: HYPED
 * Date: 06/11/2020
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

#include <iostream>
#include "math.h"
#include "gtest/gtest.h"
#include "utils/math/differentiator.hpp"
#include "utils/system.hpp"
#include "data/data_point.hpp"

using hyped::data::DataPoint;
using hyped::utils::math::Differentiator;

// -------------------------------------------------------------------------------------------------
// Functionality
// -------------------------------------------------------------------------------------------------

/**
 * Struct used for test fixtures testing functionality.
 * Constructor sets the values of the DataPoint objects.
 * Helper functions defined to assist in testing
 */
struct DifferentiatorFunctionality : public ::testing::Test {
 protected:
  Differentiator<float> diff_test;
  DataPoint<float> test_point;
  DataPoint<float> second_point;
  DataPoint<float> third_point;

  void SetUp()
  {
    // Sets values of Data points
    test_point.value = 10.5;
    test_point.timestamp = 1;

    second_point.value = 18.6;
    second_point.timestamp = 2;

    third_point.value= second_point.value;
    third_point.timestamp = 3;
  }
  void TearDown() {}
};

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

/**
 * Test fixture used for determining whether initialisation works correctly.
 * The value should always return 0
*/
TEST_F(DifferentiatorFunctionality, differentiatorInitialisedValue)
{
  ASSERT_EQ(diff_test.update(test_point).value, 0)
    << "Initalised value should be 0";
}

/**
 * Test fixture used for determining whether initialisation works correctly.
 * The timestamp should always be returned unchanged
*/
TEST_F(DifferentiatorFunctionality, differentiatorInitialisedTimestamp)
{
  ASSERT_EQ(diff_test.update(test_point).timestamp, test_point.timestamp)
    << "Timestamp of Data Point shouldnt change on update";
}

/**
 * Test fixture used for checking the correctness of the gradient between two points.
 * The Value of update should return the same as the gradient between the two points.
 */
TEST_F(DifferentiatorFunctionality, differentiatorGradient)
{
  diff_test.update(test_point);
  ASSERT_FLOAT_EQ(diff_test.update(second_point).value, gradientOfPoints(second_point, test_point))
    << "Derivative differs from the gradient between the two points";
}

/**
 * Test fixture used for ensuring timestamp remains unchanged.
 * Timestamp should always return unchanged whenever update is called.
 */
TEST_F(DifferentiatorFunctionality, differentiatorTimestampReturn)
{
  diff_test.update(test_point);
  ASSERT_EQ(diff_test.update(second_point).timestamp, second_point.timestamp)
    << "Timestamp should remain unchanged on Update";
}

/**
 * Test fixture for testing the derivative of the same value.
 * Derivative should always return as 0 since the two points are the same
 * so the gradient between them is 0.
 */
TEST_F(DifferentiatorFunctionality, derivativeOfSameValue)
{
  diff_test.update(test_point);
  diff_test.update(second_point);
  ASSERT_EQ(diff_test.update(third_point).value, 0)
    << "Derivative of same values should return 0";
}

// -------------------------------------------------------------------------------------------------
// Special of Cases
// -------------------------------------------------------------------------------------------------

/**
 * Struct used in Testing the Differentiator special cases for linear and constants.
 * Constructor populates an array of DataPoints with sample data for each of the functions.
 * These arrays are then accessed in the Test's for Test data.
 */
struct SpecialCases : public ::testing::Test {
 protected:
  Differentiator<float> diff_linear;
  DataPoint<float> linear_data[100];
  DataPoint<float> linear_data_point;
  Differentiator<float> diff_constant;
  DataPoint<float> function_constant[100];
  DataPoint<float> constant_data_point;
  const float kConstant = rand() % 100;

  void SetUp()
  {
    for (int i = 0; i < 100; i++) {
      linear_data_point = DataPoint<float>(i*pow(10, 6), i);
      linear_data[i] = linear_data_point;
      constant_data_point = DataPoint<float>(i*pow(10, 6), kConstant);
      function_constant[i] = constant_data_point;
    }
  }
};

/**
 * Test For the Linear Case, we expect a perfect fit. As the gradient of a line is always constant.
 */
TEST_F(SpecialCases, differentiatorLinearCase)
{
  diff_linear.update(linear_data[0]);
  for (int i = 1; i < 100; i++) {
    float value = diff_linear.update(linear_data[i]).value;
    ASSERT_EQ(1, value)
      << "You Expect a perfect fit for linear case, please review implementation";
  }
}

/**
 * Test For the Constant Case, we expect a perfect fit. As the gradient of this function should
 * always be 0.
 */
TEST_F(SpecialCases, differentiatorConstantCase)
{
  diff_constant.update(function_constant[0]);
  for (int i = 1; i < 100; i++) {
    float value = diff_constant.update(function_constant[i]).value;
    ASSERT_EQ(0, value)
      << "You Expect a perfect fit for constant case, please review implementation";
  }
}

// -------------------------------------------------------------------------------------------------
// Properties of derivatives
// -------------------------------------------------------------------------------------------------

/**
 * Struct used for checking the Sum of Derivatives property
 * Creates an Array of DataPoints for the linear, quadratic, and function data.
 * These arrays are later accessed in the tests for Test Data.
 */
struct DifferentiatorProperty : public ::testing::Test {
 protected:
  Differentiator<float> diff_linear;
  Differentiator<float> diff_quadratic;
  Differentiator<float> diff_function;
  DataPoint<float> linear_data[100];
  DataPoint<float> quadratic_data[100];
  DataPoint<float> function_data[100];
  DataPoint<float> difference_function[100];
  DataPoint<float> data_point;

  void SetUp()
  {
    for (int i = 0; i < 100; i++) {
      data_point = DataPoint<float>(i*pow(10, 6), i);
      linear_data[i] = data_point;
      data_point  = DataPoint<float>(i*pow(10, 6), i*i);
      quadratic_data[i] = data_point;
      data_point  = DataPoint<float>(i*pow(10, 6), i*i+i);
      function_data[i] = data_point;
      data_point = DataPoint<float>(i*pow(10, 6), i*i - i);
      difference_function[i] = data_point;
    }
  }
};

/**
 * Test fixture used for checking the sum of derivatives.
 * The derivative of the sum of two functions should always return equal to the sum
 * of the derivatives of the two functions.
 */
TEST_F(DifferentiatorProperty, differentiatorSumOfDerivatives)
{
  for (int i = 0; i < 100; i++) {
    float linear = diff_linear.update(linear_data[i]).value;
    float quadratic = diff_quadratic.update(quadratic_data[i]).value;
    float function  = diff_function.update(function_data[i]).value;

    ASSERT_EQ(function, linear+quadratic)
      << "The derivative should be the same as the sum of the two derivatives";
  }
}

/**
 * Test fixture used for checking the difference of derivatives.
 * The Test checks whether the derivative of the difference of two functions is equal
 * to the difference of their derivatives
 */
TEST_F(DifferentiatorProperty, differentiatorDifferenceOfDerivatives)
{
  for (int i = 0; i < 100; i++) {
    float linear = diff_linear.update(linear_data[i]).value;
    float quadratic = diff_quadratic.update(quadratic_data[i]).value;
    float function  = diff_function.update(difference_function[i]).value;

    ASSERT_EQ(function, quadratic-linear)
      << "The derivative should be the same as the difference of the two derivatives";
  }
}

/**
 * Test fixture for checking the Chain Rule property of derivatives.
 */
TEST(DifferentiatorChainRule, differentiatorChainRule)
{
  Differentiator<float> diff_inner;
  Differentiator<float> diff_outer;
  DataPoint<float> inner_function[100];
  DataPoint<float> outer_function[100];
  DataPoint<float> data_point;

  // Populates the array of datapoints with the output of functions
  for (int i = 0; i < 100; i++) {
    data_point  = DataPoint<float>(i*1e6, pow(2*i + 1, 2));
    inner_function[i] = data_point;
    data_point  = DataPoint<float>(i*1e6, 3 * inner_function[i].value);
    outer_function[i] = data_point;
  }

  // Iterates through the arrays testing whether the chain rule holds
  for (int i = 0; i < 100; i++) {
    float inner = diff_inner.update(inner_function[i]).value;
    float outer = diff_outer.update(outer_function[i]).value;

    ASSERT_EQ(outer, (3 * inner))
      << "Chain rule doesn't hold.";
  }
};
