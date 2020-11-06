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

/**
 * Test fixture used for determining whether initialisation works correctly.
*/
TEST_F(DifferentiatorFunctionality, differentiatorInitialisedValue)
{
  ASSERT_EQ(diff_test.update(test_point).value, 0);
}

/**
 * Test fixture used for determining whether initialisation works correctly.
*/
TEST_F(DifferentiatorFunctionality, differentiatorInitialisedTimestamp)
{
  ASSERT_EQ(diff_test.update(test_point).timestamp, test_point.timestamp);
}

/**
 * Test fixture used for checking the correctness of the gradient between two points.
 */
TEST_F(DifferentiatorFunctionality, differentiatorGradient)
{
  diff_test.update(test_point);
  ASSERT_FLOAT_EQ(diff_test.update(second_point).value, gradientOfPoints(second_point, test_point));
}

/**
 * Test fixture used for ensuring timestamp remains unchanged.
 */
TEST_F(DifferentiatorFunctionality, differentiatorTimestampReturn)
{
  diff_test.update(test_point);
  ASSERT_EQ(diff_test.update(second_point).timestamp, second_point.timestamp);
}

/**
 * Test fixture for testing the derivative of the same value of timestamps.
 */
TEST_F(DifferentiatorFunctionality, derivativeOfSameValue)
{
  diff_test.update(test_point);
  diff_test.update(second_point);
  ASSERT_EQ(diff_test.update(third_point).value, 0);
}

// -------------------------------------------------------------------------------------------------
// Special of Cases
// -------------------------------------------------------------------------------------------------

/**
 * Struct used as text fiture, it will be used to test the linear case.
 * Therefore it will create a list of DataPoints of the function y = x.
 */
struct DifferentiatorLinear : public ::testing::Test {
  protected:
    Differentiator<float> diff_linear;
    DataPoint<float> linear_data[100];
    DataPoint<float> linear_data_point;
    void SetUp()
    {
      for (int i = 0; i < 100; i++) {
        linear_data_point = DataPoint<float>(i*pow(10, 6), i);
        linear_data[i] = linear_data_point;
      }
    }
};

/**
 * Test For the Linear Case, we expect a perfect fit. As the gradient of a line is always constant.
 */
TEST_F(DifferentiatorLinear, differentiatorLinearCase)
{
  diff_linear.update(linear_data[0]);
  for (int i = 1; i < 100; i++) {
    float value = diff_linear.update(linear_data[i]).value;
    ASSERT_EQ(1, value) << "You Expect a perfect fit for linear case, please review implementation";
  }
}

/**
 * Struct used as text fixture, it will be used to test the constant case.
 * Therefore it will create a list of DataPoints of the function y = a.
 */
struct DifferentiatorConstant : public :: testing::Test {
  protected:
    Differentiator<float> diff_constant;
    DataPoint<float> function_constant[100];
    DataPoint<float> constant_data_point;
    const float constant = 10;
    void SetUp()
    {
      for (int i = 0; i < 100; i++) {
        constant_data_point = DataPoint<float>(i*pow(10, 6), constant);
        function_constant[i] = constant_data_point;
      }
    }
};

/**
 * Test For the Constant Case, we expect a perfect fit. As the gradient of this function should
 * always be 0.
 */
TEST_F(DifferentiatorConstant, differentiatorConstantCase)
{
  diff_constant.update(function_constant[0]);
  for (int i = 1; i < 100; i++) {
    float value = diff_constant.update(function_constant[i]).value;
    ASSERT_EQ(0, value) << "You Expect a perfect fit for linear case, please review implementation";
  }
}

// -------------------------------------------------------------------------------------------------
// Properties of derivatives
// -------------------------------------------------------------------------------------------------

/**
 * Struct used for checking the Sum of Derivatives property
 * Creates a list of DataPoint for the linear, quadratic, and function data.
 */
struct DifferentiatorProperty : public ::testing::Test {
  protected:
    Differentiator<float> diff_linear;
    Differentiator<float> diff_quadratic;
    Differentiator<float> diff_function;
    DataPoint<float> linear_data[100];
    DataPoint<float> quadratic_data[100];
    DataPoint<float> function_data[100];
    DataPoint<float> data_point;

    // Populates the lists with the output of their functions
    void SetUp()
    {
      for (int i = 0; i < 100; i++) {
        data_point = DataPoint<float>(i*pow(10, 6), i);
        linear_data[i] = data_point;
        data_point  = DataPoint<float>(i*pow(10, 6), i*i);
        quadratic_data[i] = data_point;
        data_point  = DataPoint<float>(i*pow(10, 6), i*i+i);
        function_data[i] = data_point;
      }
    }
};

/**
 * Test fixture used for checking the sum of derivatives.
 */
TEST_F(DifferentiatorProperty, differentiatorSumOfDerivatives)
{
  for (int i = 0; i < 100; i++) {
    float linear = diff_linear.update(linear_data[i]).value;
    float quadratic = diff_quadratic.update(quadratic_data[i]).value;
    float function  = diff_function.update(function_data[i]).value;

    ASSERT_EQ(function, linear+quadratic);
  }
}

/**
 * Struct used in Testing the difference of derivatives. Similar
 * to the struct used for finding the sum except it finds the
 * difference .
 */
struct DifferentiatorDifference : public ::testing::Test {
  protected:
    Differentiator<float> diff_linear;
    Differentiator<float> diff_quadratic;
    Differentiator<float> diff_function;
    DataPoint<float> linear_data[100];
    DataPoint<float> quadratic_data[100];
    DataPoint<float> function_data[100];
    DataPoint<float> data_point;

    // Populates the lists with their functions outputs.
    void SetUp()
    {
      for (int i = 0; i < 100; i++) {
        data_point = DataPoint<float>(i*pow(10, 6), i);
        linear_data[i] = data_point;
        data_point  = DataPoint<float>(i*pow(10, 6), i*i);
        quadratic_data[i] = data_point;
        data_point  = DataPoint<float>(i*pow(10, 6), i*i-i);
        function_data[i] = data_point;
      }
    }
};

/**
 * Test fixture used for checking the difference of derivatives.
 */
TEST_F(DifferentiatorDifference, differentiatorDifferenceOfDerivatives)
{
  for (int i = 0; i < 100; i++) {
    float linear = diff_linear.update(linear_data[i]).value;
    float quadratic = diff_quadratic.update(quadratic_data[i]).value;
    float function  = diff_function.update(function_data[i]).value;

    ASSERT_EQ(function, quadratic-linear);
  }
}

/**
 * Struct used for testing that the chain rule applies in the
 * differentiator. Creates 2 lists of DataPoints for the outer
 * and inner functions and stores their DataPoints.
 */
struct DifferentiatorChainRule : public ::testing::Test {
    protected:
    Differentiator<float> diff_inner;
    Differentiator<float> diff_outer;
    DataPoint<float> inner_function[100];
    DataPoint<float> outer_function[100];
    DataPoint<float> data_point;

    // Populates the lists with their function's outputs
    void SetUp()
    {
      for (int i = 0; i < 100; i++) {
        data_point  = DataPoint<float>(i*1e6, pow(2*i + 1, 2));
        inner_function[i] = data_point;
        data_point  = DataPoint<float>(i*1e6, 3 * inner_function[i].value);
        outer_function[i] = data_point;
      }
    }
};

/**
 * Test fixture for checking the Chain Rule property of derivatives.
 */
TEST_F(DifferentiatorChainRule, differentiatorChainRule)
{
  for (int i = 0; i < 100; i++) {
    float inner = diff_inner.update(inner_function[i]).value;
    float outer = diff_outer.update(outer_function[i]).value;

    ASSERT_EQ(outer, (3 * inner));
  }
};
