/*
 * Author: Pablo Morandé
 * Organisation: HYPED
 * Date: 13/11/2020
 * Description: Testing file for Integrator.hpp
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
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include "gtest/gtest.h"
#include "utils/math/integrator.hpp"
#include "data/data_point.hpp"

namespace hyped {
namespace utils {
namespace math {

// -------------------------------------------------------------------------------------------------
// Helper Functions
// -------------------------------------------------------------------------------------------------
/**
 * @brief Helper function used to divide an interval in a number of points.
 * evenly spaced.
 * Does not work for integers.
 *
 */
template <typename T>
std::vector<T> linspace(T a, T b, size_t n)
{
  T h = (b - a) / static_cast<T>(n - 1);
  std::vector<T> xs(n);
  typename std::vector<T>::iterator x;
  T val;
  for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h) *x = val;
  return xs;
}
// -------------------------------------------------------------------------------------------------
// Functionality Tests
// -------------------------------------------------------------------------------------------------
/**
 * @brief Test designed to see if the integraotr is initialized properly.
 * It will check if the first value that updates the integrator just changes the lower bound of the
 * integral.
 */
TEST(IntegratorFunctionalityTest, handlesInitializationTest)
{
  DataPoint<float> firstPoint= DataPoint<float>(pow(10, 6), 10);
  DataPoint<float> output = DataPoint<float>(0, 0);
  Integrator<float> integrator =  Integrator<float>(&output);
  integrator.update(firstPoint);
  std::string message_one = "The first value should just change the lower bound of the integral, "
  "it should not affect the total area. Please review implementation";
  ASSERT_EQ(0, output.value) << message_one;
  std::string message_two = "The timeStep should change to the one given by "
  "the first point, Plese review implementation";
  ASSERT_EQ(pow(10, 6), output.timestamp) << message_two;
}

// -------------------------------------------------------------------------------------------------
// Property Tests
// -------------------------------------------------------------------------------------------------

/**
 * @brief struct used to setup all needed variables to make property tests of Integrator
 *
 */
struct IntegratorPropertyTest : public ::testing:: Test
{
  int number_of_points = 101;
  DataPoint<float> datafunction[101];

  DataPoint<float> dataxcube[101];
  DataPoint<float> dataquadratic[101];
  DataPoint<float> datax[101];
  DataPoint<float> function;
  DataPoint<float> cubeterm;
  DataPoint<float> quadraticterm;
  DataPoint<float> linerarterm;
  std::string message =
  "This values should be equal as the property of addition holds for any function,"
  "Please review the current implementation.The tested code does not represent a good integral";

  void SetUp()
  {
    for (int i = 0; i < number_of_points; i++) {
      function = DataPoint<float>(i * pow(10, 6), i * i * i + i * i + i);
      cubeterm = DataPoint<float>(i * pow(10, 6), i * i * i);
      quadraticterm = DataPoint<float>(i * pow(10, 6), i * i);
      linerarterm = DataPoint<float>(i * pow(10, 6), i);
      datafunction[i] = function;
      dataquadratic[i] = quadraticterm;
      dataxcube[i] = cubeterm;
      datax[i] = linerarterm;
    }
  }
};
/**
 * @brief Test designed to check if one of the most important properties of integrals holds with
 * the given implementation. The addition property
 * If I have a function like f(x) = x^2 +x then the definite integral of f(x) is the same as
 * the integral of x^2 added to the integral of x.
 * This should hold for any function f(x)
 */
TEST_F(IntegratorPropertyTest, AdditionPropertyTest)
{
  DataPoint<float> velocity = DataPoint<float>(0, 0);
  Integrator<float> function_integrator = Integrator<float>(&velocity);
  DataPoint<float> velocity_2 = DataPoint<float>(0, 0);
  Integrator<float> quadratic_integrator = Integrator<float>(&velocity_2);
  DataPoint<float> velocity_3 = DataPoint<float>(0, 0);
  Integrator<float> linear_integrator = Integrator<float>(&velocity_3);
  DataPoint<float> velocity_4 = DataPoint<float>(0, 0);
  Integrator<float> cube_integrator = Integrator<float>(&velocity_4);
  for (int i = 0; i < number_of_points; i++) {
    function_integrator.update(datafunction[i]);
    quadratic_integrator.update(dataquadratic[i]);
    linear_integrator.update(datax[i]);
    cube_integrator.update(dataxcube[i]);
  }
  ASSERT_EQ(velocity.value, velocity_2.value + velocity_3.value+velocity_4.value) << message;
}
/**
 * @brief Test designed to check if one of the most important properties of integrals
 * and it should hold with the given implementation. The Interval property.
 * If I have a function f(x) then the definite integral of f(x) over a to b is the same as
 * the integral of f(x) over a to b/2 added to the integral of f(x) over b/2 to b.
 * This should hold for any function f(x)
 */
TEST_F(IntegratorPropertyTest, IntervalTest)
{
  DataPoint<float> reference1;
  DataPoint<float> velocity = DataPoint<float>(0, 0);
  Integrator<float> function_integrator = Integrator<float>(&velocity);
  DataPoint<float> reference2;
  DataPoint<float> velocity_2 = DataPoint<float>(0, 0);
  Integrator<float> interval_1_integrator = Integrator<float>(&velocity_2);
  DataPoint<float> reference3;
  DataPoint<float> velocity_3 = DataPoint<float>(0, 0);
  Integrator<float> interval_2_integrator = Integrator<float>(&velocity_3);

  for (int i = 0; i < 101; i++) {
    function_integrator.update(datafunction[i]);
  }
  for (int i = 0; i < 50; i++) {
    interval_1_integrator.update(datafunction[i]);
  }
  for (int i = 49; i < 101; i++) {
    interval_2_integrator.update(datafunction[i]);
  }
  ASSERT_EQ(velocity.value, velocity_2.value + velocity_3.value) << message;
}
/**
 * @brief Test designed to check if one of the most important properties of integrals holds
 *  with the given implementation. The Constant property.
 * If I have a function f(x)= C*g(x) where C is a constant
 * then the definite integral of f(x) over a to b is the same as the integral of g(x) from a to b
 * multiplied by the constant C
 * This should hold for any function of the form of f(x) = C*g(x).
 */
TEST_F(IntegratorPropertyTest, ConstantMultiplyTest)
{
  int constant = 10;
  DataPoint<float> velocity = DataPoint<float>(0, 0);
  Integrator<float> function_integrator = Integrator<float>(&velocity);
  DataPoint<float> velocity_2 = DataPoint<float>(0, 0);
  Integrator<float> function_integrator_multiplied_by_constant = Integrator<float>(&velocity_2);
  DataPoint<float> data_function_by_constant[101];
  for (int i = 0; i < 101; i++) {
    float value = datafunction[i].value;
    data_function_by_constant[i] = DataPoint<float>(i*pow(10, 6), constant*value);
  }
  for (int i = 0; i < 101; i++) {
    function_integrator.update(datafunction[i]);
    function_integrator_multiplied_by_constant.update(data_function_by_constant[i]);
  }
  ASSERT_EQ(constant * velocity.value, velocity_2.value) << message;
}
// -------------------------------------------------------------------------------------------------
// Linear Tests
// -------------------------------------------------------------------------------------------------
/**
 * @brief struct used to set up all needed variables to make linear tests of Integrator
 *
 */
struct IntegratorTestLinear : public ::testing::Test {
  int max_time =100;
  DataPoint<float> datatry[101];
  DataPoint<float> datatry2[101];
  DataPoint<float> datatimeAndAcc;
  DataPoint<float> datatimeAndAcc2;
  int kConstant =rand()%100;
  std::string message = "Expected Perfect fit in linear case -> Urgent: modify Implementation";

  void SetUp()
  {
    for (int i = 0; i < 101; i++) {
      datatimeAndAcc = DataPoint<float>(i * pow(10, 6), i);
      datatry[i] = datatimeAndAcc;
    }
    for (int j = 0; j < 101; j++) {
      datatimeAndAcc = DataPoint<float>(j * pow(10, 6), kConstant * j);
      datatry2[j] = datatimeAndAcc;
    }
  }
  void TearDown() {}
};
/**
 * @brief Test used to verify that we get a perfect fit in the case of a linear function of the form
 * f(x) = x
 * If we don't get this perfect fit the integrator is doing something wrong
 * getting the area under a line should be trivial.
 */
TEST_F(IntegratorTestLinear, linearAreaTest)
{
  DataPoint <float> reference;
  DataPoint <float> velocity = DataPoint<float>(0, 0);
  Integrator<float> integratetry = Integrator<float>(&velocity);
  for (int i = 0; i < 101; i++) {
    reference = integratetry.update(datatry[i]);
  }
  float expected_value = max_time * max_time / 2;
  ASSERT_EQ(expected_value, reference.value) << message;
}
/**
 * @brief Test used to verify that we get a perfect fit in the case of a linear function of the form
 * f(x) = C*x
 * If we don't get this perfect fit the integrator is doing something wrong
 * getting the area under a line should be trivial.
 */
TEST_F(IntegratorTestLinear, linearAreaTest2)
{
  DataPoint<float> reference;
  DataPoint<float> velocity = DataPoint<float>(0, 0);
  Integrator<float> integratetry = Integrator<float>(&velocity);
  for (int i = 0; i < 101; i++) {
    reference = integratetry.update(datatry2[i]);
  }
  float expected_value =kConstant * max_time * max_time / 2;
  ASSERT_EQ(expected_value, reference.value) << message;
}

// -------------------------------------------------------------------------------------------------
// Quadratic Tests
// -------------------------------------------------------------------------------------------------
/**
 * @brief Test used to verify that we get a expect a good fit in the case of a quadratic function
 * of the form f(x) = x^2
 * If we don't get this good fit the integrator is doing something wrong
 * as We are still close to the case y = x.
 */
TEST(IntegratorTestQuadratic, QuadraticTest)
{
  int number_of_points = 10001;
  int max_time = 100;
  DataPoint<float> data_to_integrate[10001];
  std::vector<float> data_time;
  DataPoint<float> datatimeAndAcc;
  std::vector<float>::const_iterator t;
  int count = 0;
  data_time = linspace<float>(0, max_time, number_of_points);
  float timer;
  for (t = data_time.begin(); t != data_time.end(); t++) {
    timer = *t;
    datatimeAndAcc = DataPoint<float>(timer * pow(10, 6), timer * timer);
    data_to_integrate[count] = datatimeAndAcc;
    count++;
  }
  std::string message = "Bad approximation, please modify implementation";
  DataPoint<float> reference;
  DataPoint<float> velocity = DataPoint<float>(0, 0);
  Integrator<float> integratetry = Integrator<float>(&velocity);
  for (int i = 0; i < 10001; i++) {
    reference = integratetry.update(data_to_integrate[i]);
  }
  ASSERT_EQ(reference.value <= 333333 + 50 && reference.value >= 333333 - 50, true) << message;
}
}
}}
