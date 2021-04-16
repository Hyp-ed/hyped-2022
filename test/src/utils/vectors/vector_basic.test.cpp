/*
 * Author: Pablo Morandé & Flori
 * Organisation: HYPED
 * Date: 13/11/2020
 * Description: Testing file for vector.cpp
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
#include <cstdlib>
#include "gtest/gtest.h"
#include "utils/math/vector.hpp"



namespace hyped
{
namespace utils
{
namespace math
{

/**
 * @brief Helper method used to generate an array of three random integers.
 * each integer will be in the range -1000, 1000 (not inclusive)
 */
std::array<int, 3> createRandomArrayForBasic()
{
  std::array<int, 3> output = std::array<int, 3>();
  for (int i = 0;i < 3;i++) {
    output[i] = rand()%1000;
  }
  return output;
}

/**
 * @brief Struct used to set up all the variables used in the tests for basic operationsof a vector
 * with constants.
 * This tests will check if that basic operations with constants are performed the way  they should.
 * However, they do not test properties.
 */
struct OperationsByConstant : public::testing::Test
{
  const int dimension = 3;
  Vector<int, 3> vector_one;
  Vector<int, 3> vector_two;
  Vector<int, 3> vector_three;
  Vector<int, 3> vector_result_one;
  Vector<int, 3> vector_result_two;
  int kValue = rand()%1000;
  void SetUp()
  {
    std::srand(time(0));
    vector_one = Vector<int, 3>(createRandomArrayForBasic());
    vector_two = Vector<int, 3>(createRandomArrayForBasic());
    vector_three = Vector<int, 3>(createRandomArrayForBasic());
    vector_result_one = Vector<int, 3>();
    vector_result_two = Vector<int, 3>();
  }
};

/**
 * @brief Test used to determine if the current implementation of the class vector allows adding in
 * a vector with a constant in normal way (adding the constant to each element),
 * this tests the operator (+).
 * The test will be performed using one random 3D vector and a random constant.
 */
TEST_F(OperationsByConstant, handlesAutoAdditionWithConstant)
{
  vector_result_one = vector_one + kValue;
  vector_result_two = kValue + vector_one;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], vector_one[i] + kValue);
    ASSERT_EQ(vector_result_two[i], vector_one[i] + kValue);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector allows adding
 * a vector with a constant in place (updating the initial vector) in a normal way
 * (adding the constant to each element), this tests the operator (+=).
 * The test will be performed using one random 3D vector and a random constant.
 */
TEST_F(OperationsByConstant, handlesAdditionWithConstant)
{
  vector_result_one += kValue;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], kValue);
  }
  vector_result_one += vector_one;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], vector_one[i] + kValue);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * substracting a constant to a vector in normal way (substracting the constant to each element),
 * this tests the operator (-).
 * The test will be performed using one random 3D vector and a random constant.
 */
TEST_F(OperationsByConstant, handlesAutoSubstractionWithConstant)
{
  vector_result_one = vector_one - kValue;
  vector_result_two = kValue - vector_one;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], vector_one[i] - kValue);
    ASSERT_EQ(vector_result_two[i], -vector_one[i] + kValue);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * substracting a constant to a vector in place (updating the initial vector)
 * in normal way (substracting the constant to each element),
 * this tests the operator (-=).
 * The test will be performed using one random 3D vector and a random constant.
 */
TEST_F(OperationsByConstant, handlesSubstractionWithConstant)
{
  vector_result_one -= kValue;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], -kValue);
  }
  vector_result_one += vector_one;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], vector_one[i] - kValue);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * multiplying a constant to a vector in normal way (multiplying the constant with each element),
 * this tests the operator (*).
 * The test will be performed using one random 3D vector and a random constant.
 */
TEST_F(OperationsByConstant, handlesAutoMultiplicationWithConstant)
{
  vector_result_one = vector_result_one * kValue;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], 0);
  }
  vector_result_one = vector_one * kValue;
  vector_result_two = kValue * vector_one;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], vector_one[i] * kValue);
    ASSERT_EQ(vector_result_two[i], vector_one[i] * kValue);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * multiplying a constant to a vector in place (updating the initial vector)
 * in normal way (multiplying the constant with each element),
 * this tests the operator (*=).
 * The test will be performed using one random 3D vector and a random constant.
 */
TEST_F(OperationsByConstant, handlesMultiplicationWithConstant)
{
  vector_result_one *= kValue;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], 0);
  }
  vector_result_one = Vector<int, 3>(1);
  vector_result_one *= kValue;
  vector_result_one *= vector_one;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], vector_one[i] * kValue);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * multiplying a constant to a vector in normal way (dividing each element by the constant),
 * this tests the operator (=).
 * The test will be performed using one random 3D vector and a random constant.
 * It is important to be sure that the constant is not zero to avoid
 * division 0/0
 */
TEST_F(OperationsByConstant, handlesAutoDivisionWithConstant)
{
  while (kValue == 0) {
    kValue = rand() % 1000;
  }
  vector_result_one = vector_result_one / kValue;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], 0);
  }
  vector_result_one = vector_one / kValue;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], static_cast<int>(vector_one[i]/ kValue));
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * multiplying a constant to a vector in place (updating the initial vector)
 * in normal way (dividing each element by the constant),
 * this tests the operator (/=).
 * The test will be performed using one random 3D vector and a random constant.
 * It is important to be sure that the constant is not zero to avoid
 * division 0/0
 */
TEST_F(OperationsByConstant, handlesDivisionWithConstant)
{
  while (kValue == 0) {
    kValue = rand() % 1000;
  }
  vector_result_one /= kValue;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], 0);
  }
  vector_result_one = vector_one;
  vector_result_one /= kValue;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], static_cast<int>(vector_one[i]/ kValue));
  }
}
}}}  // hyped::utils::math
