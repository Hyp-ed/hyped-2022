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

#include <iostream>
#include "math.h"
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


struct differentiator_linear_case : public ::testing::Test {
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

struct property_differentiator : public ::testing::Test {
  protected:
    Differentiator<float> diff_linear;
    Differentiator<float> diff_quadratic;
    Differentiator<float> diff_function;
    DataPoint<float> linear_data[100];
    DataPoint<float> quadratic_data[100];
    DataPoint<float> function_data[100];
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
      }
    }
};  

struct difference_differentiator : public ::testing::Test {
    protected:
    Differentiator<float> diff_linear;
    Differentiator<float> diff_quadratic;
    Differentiator<float> diff_function;
    DataPoint<float> linear_data[100];
    DataPoint<float> quadratic_data[100];
    DataPoint<float> function_data[100];
    DataPoint<float> data_point;
    
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

struct differentiator_product_rule : public ::testing::Test {
    protected:
    Differentiator<float> diff_square;
    Differentiator<float> diff_double;
    Differentiator<float> diff_function;
    DataPoint<float> square_data[100];
    DataPoint<float> double_data[100];
    DataPoint<float> function_data[100];
    DataPoint<float> data_point;
    
    float square_function(int x) { return pow(x, 2); }
    float double_function(int x) {return x*2; }
    
    void SetUp()
    {
      for (int i = 0; i < 100; i++) {
        data_point = DataPoint<float>(i*pow(10, 6), square_function(i));
        square_data[i] = data_point;
        data_point  = DataPoint<float>(i*pow(10, 6), double_function(i));
        double_data[i] = data_point;
        data_point  = DataPoint<float>(i*pow(10, 6), square_function(i) * double_function(i));
        function_data[i] = data_point;
      }
    }  
};

struct differentiator_chain_rule : public ::testing::Test {
    protected:
    Differentiator<float> diff_inner;
    Differentiator<float> diff_outer;
    DataPoint<float> inner_function[100];
    DataPoint<float> outer_function[100];
    DataPoint<float> data_point;
    
    void SetUp()
    {
      for (int i = 0; i < 100; i++) {
        data_point  = DataPoint<float>(i*1e6, (2*i + 1)^2);
        inner_function[i] = data_point;
        data_point  = DataPoint<float>(i*1e6, 3 * inner_function[i].value);
        outer_function[i] = data_point;
      }
    }  
};
struct differentiator_quotient_rule : public ::testing::Test {
    protected:
    Differentiator<float> diff_numerator;
    Differentiator<float> diff_denominator;
    Differentiator<float> diff_fraction;
    DataPoint<float> numerator_data[100];
    DataPoint<float> denominator_data[100];
    DataPoint<float> fraction_data[100];
    DataPoint<float> data_point;
    float numerator_function(int x) { return 3*x; }
    float denominator_function(int x) { return 2*x; }
    float fractional_function(int n, int d) { 
      return (numerator_function(n) / denominator_function(d));
    }
    
    void SetUp()
    {
      for (int i = 1; i < 100; i++) {
        data_point  = DataPoint<float>(i*1e6, numerator_function(i));
        numerator_data[i] = data_point;
        data_point  = DataPoint<float>(i*1e6, denominator_function(i));
        denominator_data[i] = data_point;
        data_point = DataPoint<float>(i*1e6, fractional_function(i, i));
        fraction_data[i] = data_point;
      }
    }  
};
// Test fixture sfor determining whether initalisation works correctly
TEST_F(differentiator_test, DifferentiatorInitialisedValue)
{ 
  ASSERT_EQ(diff_test.update(test_point).value, 0);
};

TEST_F(differentiator_test, DifferentiatorInitialisedTimestamp)
{
  ASSERT_EQ(diff_test.update(test_point).timestamp, test_point.timestamp);
}

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

// Test fixture for testing the derivative of the same value of timestamps.
TEST_F(differentiator_test, DerivativeOfSameValue)
{
  diff_test.update(test_point);
  diff_test.update(second_point);
  ASSERT_EQ(diff_test.update(third_point).value, 0);
}

// Properties of Diff

// Linear Case
TEST_F(differentiator_linear_case, DifferentiatorLinearCase)
{
  diff_linear.update(linear_data[0]);
  for (int i = 1; i < 100; i++) {
    float value = diff_linear.update(linear_data[i]).value;
    ASSERT_EQ(1, value) << "You Expect a perfect fit for linear case, please review implementation";
  }
}

// Sum of derivatives
TEST_F(property_differentiator, DifferentiatorSumOfDerivatives)
{
  for (int i = 0; i < 100; i++) {
    float linear = diff_linear.update(linear_data[i]).value;
    float quadratic = diff_quadratic.update(quadratic_data[i]).value;
    float function  = diff_function.update(function_data[i]).value;
    
    ASSERT_EQ(function, linear+quadratic);
  }
}
  
// Difference of derivatives
TEST_F(difference_differentiator, DifferentiatorDifferenceOfDerivatives)
{
  for (int i = 0; i < 100; i++) {
    float linear = diff_linear.update(linear_data[i]).value;
    float quadratic = diff_quadratic.update(quadratic_data[i]).value;
    float function  = diff_function.update(function_data[i]).value;
    
    ASSERT_EQ(function, quadratic-linear);
  }
}

/*
TEST_F(differentiator_product_rule, DifferentiatorProductRule)
{
  for (int i = 0; i < 100; i++) {
    float square = diff_square.update(square_data[i]).value;
    float Double = diff_double.update(double_data[i]).value;
    float function  = diff_function.update(function_data[i]).value;
    ASSERT_EQ(function, (square * double_function(i)) + (square_function(i) * Double));
  }
}
*/
TEST_F(differentiator_chain_rule, DifferentiatorChainRule)
  {
  for (int i = 0; i < 100; i++) {
    float inner = diff_inner.update(inner_function[i]).value;
    float outer = diff_outer.update(outer_function[i]).value;

    ASSERT_EQ(outer, (3 * inner));
  }
  };

/*
TEST_F(differentiator_quotient_rule, DifferentiatorQuotientRule)
{
  diff_numerator.update(numerator_data[0]);
  diff_denominator.update(denominator_data[0]);
  diff_fraction.update(fraction_data[0]);

  for (int i = 1; i < 100; i++){
    float numerator = diff_numerator.update(numerator_data[i]).value;
    float denominator = diff_denominator.update(denominator_data[i]).value;
    float fraction = diff_fraction.update(fraction_data[i]).value;

    std::cout << "NumData : " << numerator_data[i].value << " | DenomData :" << denominator_data[i].value << " | FractionData :" << fraction_data[i].value << "\n";
    std::cout << "Num : " << numerator << " | Denom : " << denominator << " | Fraction : " << fraction << "\n";

    float quotient_rule = ((numerator * denominator_function(i)) - (denominator * numerator_function(i)))/(pow(denominator_function(i),2.0));
    ASSERT_FLOAT_EQ(fraction, quotient_rule);
  }
};
*/
