#include <math.h>
#include <cstdlib>
#include "gtest/gtest.h"
#include "utils/math/vector.hpp"
#include "vector_basic.test.cpp"
/*
 * Author: Pablo Morandé
 * Organisation: HYPED
 * Date: 13/11/2020
 * Description: Testing file for vector.hpp
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
namespace hyped
{
namespace utils
{
namespace math
{
/**
 * @brief Struct used to set up all the variables used in the tests one porperty present in some
 * of the operations defined for vectors
 * Associativity.
 * This tests will check if Associativity holds in multiplication and addition.
 */
struct Associativity : public::testing::Test
{
  const int dimension = 3;
  Vector<int, 3> vector_one;
  Vector<int, 3> vector_two;
  Vector<int, 3> vector_three;
  Vector<int, 3> vector_result_one;
  Vector<int, 3> vector_result_two;
  void SetUp()
{
    vector_one = Vector<int, 3>(createRandomArray());
    vector_two = Vector<int, 3>(createRandomArray());
    vector_three = Vector<int, 3>(createRandomArray());
    vector_result_one = Vector<int, 3>();
    vector_result_two = Vector<int, 3>();
  }
};
/**
 * Test used to check if the operator (+=) is associative given three random 3D vectors.
 * (a + b) + c = a + (b + c).
 */
TEST_F(Associativity, isAdditionAssociative)
{
  vector_result_one += vector_one;
  vector_result_one += vector_two;
  vector_result_one += vector_three;

  vector_result_two += vector_two;
  vector_result_two += vector_three;
  vector_result_two += vector_one;
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(vector_result_two[i], vector_result_one[i]);
  }
}
/**
 * Test used to check if the operator (+) is associative given three random 3D vectors.
 * (a + b) + c = a + (b + c).
 */
TEST_F(Associativity, isAutoAdditionAssociative)
{
  vector_result_one = (vector_one+ vector_two) + vector_three;
  vector_result_two = vector_one + (vector_two+vector_three);
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(vector_result_two[i], vector_result_one[i]);
  }
}
/**
 * Test used to check if the operator (*=) is associative given three random 3D vectors.
 * (a * b) * c = a * (b * c).
 */
TEST_F(Associativity, isMultiplicationAssociative)
{
  vector_result_one *= vector_one;
  vector_result_one *= vector_two;
  vector_result_one *= vector_three;
  vector_result_two *= vector_two;
  vector_result_two *= vector_three;
  vector_result_two *= vector_one;
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(vector_result_two[i], vector_result_one[i]);
  }
}
/**
 * Test used to check if the operator (*) is associative given three random 3D vectors.
 * (a * b) * c = a * (b * c).
 */
TEST_F(Associativity, isAutoMultiplicationAssociative)
{
  vector_result_one = (vector_one * vector_two) * vector_three;
  vector_result_two = vector_one * (vector_two * vector_three);
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(vector_result_two[i], vector_result_one[i]);
  }
}
/**
 * @brief Struct used to set up all the variables used in the tests for checking if
 * one porperty is resent in some of the operations defined for vectors
 * Commutativity.
 * This tests will check if Commutativity holds in multiplication and addition.
 * Also checks that substraction is anticommutative.
 */
struct Commutativity : public ::testing::Test
{
  const int dimension = 3;
  Vector<int, 3> vector_one;
  Vector<int, 3> vector_two;
  Vector<int, 3> vector_result_one;
  Vector<int, 3> vector_result_two;
  void SetUp()
  {
    vector_one = Vector<int, 3>(createRandomArray());
    vector_two = Vector<int, 3>(createRandomArray());
    vector_result_one = Vector<int, 3>();
    vector_result_two = Vector<int, 3>();
  }
};
/**
 * Test used to check if the operator (+=) is commutative given two random 3D vectors.
 * a + b = b + a.
 */
TEST_F(Commutativity, isAdditionCommutative)
{
  vector_result_one += vector_one;
  vector_result_one += vector_two;
  vector_result_two += vector_two;
  vector_result_two += vector_one;
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(vector_result_two[i], vector_result_one[i]);
  }
}
/**
 * Test used to check if the operator (+) is commutative given two random 3D vectors.
 * a + b = b + a.
 */
TEST_F(Commutativity, isAutoAdditionCommutative)
{
  vector_result_one = vector_two + vector_one;
  vector_result_two = vector_one + vector_two;
  for (int i = 0;i < dimension;i++) {
  ASSERT_EQ(vector_result_one[i], vector_result_two[i]);
  }
}
/**
 * Test used to check if the operator (*=) is commutative given two random 3D vectors.
 * a * b = b * a.
 */
TEST_F(Commutativity, isMultiplicationCommutative)
{
  vector_result_one *= vector_two;
  vector_result_one *= vector_one;
  vector_result_two *= vector_one;
  vector_result_two *= vector_two;
  for (int i = 0;i < dimension;i++) {
  ASSERT_EQ(vector_result_one[i], vector_result_two[i]);
  }
}
/**
 * Test used to check if the operator (*) is commutative given two random 3D vectors.
 * a * b = b * a.
 */
TEST_F(Commutativity, isAutoMultiplicationCommutative)
{
  vector_result_one = vector_two * vector_one;
  vector_result_two = vector_one * vector_two;
  for (int i = 0;i < dimension;i++) {
  ASSERT_EQ(vector_result_one[i], vector_result_two[i]);
  }
}
/**
 * Test used to check if the operator (-=) is anticommutative given two random 3D vectors.
 * a - b = -(b - a).
 */
TEST_F(Commutativity, isSubstractionNotCommutative)
{
  vector_result_one += vector_one;
  vector_result_one -= vector_two;
  vector_result_two += vector_two;
  vector_result_two -= vector_one;
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(vector_result_two[i], -vector_result_one[i]);
  }
}
/**
 * Test used to check if the operator (-) is anticommutative given two random 3D vectors.
 * a - b = -(b - a).
 */
TEST_F(Commutativity, isAutoSubstractionNotCommutative)
{
  vector_result_one = vector_two - vector_one;
  vector_result_two = vector_one - vector_two;
  for (int i = 0;i < dimension;i++) {
  ASSERT_EQ(vector_result_one[i], -vector_result_two[i]);
  }
}

struct IdentityOperations : public ::testing::Test
{
  std::array<int, 3> values = createRandomArray();
  const int dimension = 3;
  Vector<int, 3> identity_vector;
  Vector<int, 3> vector;
  void SetUp()
  {
    identity_vector = Vector<int, 3>();
    vector = Vector<int, 3>(values);
  }
};

TEST_F(IdentityOperations, handlesAdditionIdentity)
{
  vector+=identity_vector;
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(vector[i], values[i]);
  }
}
TEST_F(IdentityOperations, handlesSubstractionIdentities)
{
  vector -= identity_vector;
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(vector[i], values[i]);
  }
  vector -= vector;
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(vector[i], identity_vector[i]);
  }
}
TEST_F(IdentityOperations, hadlesAutoAdditonIdentity)
{
  Vector<int, 3> output = vector + identity_vector;
  for (int i = 0; i < dimension;i++) {
    ASSERT_EQ(vector[i], output[i]);
  }
}
TEST_F(IdentityOperations, handlesAutoSubstractionIdentities)
{
  Vector<int, 3> output = vector - identity_vector;
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(output[i], values[i]);
  }
  output = vector - vector;
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(output[i], identity_vector[i]);
  }
}
TEST_F(IdentityOperations, handlesAutoMultiplicationIdentities)
{
  identity_vector = Vector<int, 3>(1);
  Vector<int, 3> output = vector * identity_vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(output[i], vector[i]);
  }
  identity_vector = Vector<int, 3>();
  output = vector * identity_vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(output[i], 0);
  }
}TEST_F(IdentityOperations, handlesMultiplicationIdentities)
{
  identity_vector = Vector<int, 3>(1);
  vector *= identity_vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector[i], values[i]);
  }
  identity_vector = Vector<int, 3>();
  vector *= identity_vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector[i], 0);
  }
}
TEST_F(IdentityOperations, handlesChangeOfSignIdentity)
{
  identity_vector = -identity_vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(identity_vector[i], 0);
  }
  Vector<int, 3> vector_two = -vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(identity_vector[i], vector_two[i] + vector[i]);
  }
}
TEST_F(IdentityOperations, handlesAutoDivisionIdentities)
{
  vector = Vector<int, 3>(createRandomWithoutZeroesArray());
  Vector<int, 3> output =  identity_vector / vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(output[i], identity_vector[i]);
  }
  identity_vector = Vector<int, 3>(1);
  output =  vector/vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(output[i], identity_vector[i]);
  }
  output =  vector/identity_vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector[i], output[i]);
  }
}
TEST_F(IdentityOperations, handlesDivisionIdentities)
{
  vector = Vector<int, 3>(createRandomWithoutZeroesArray());
  identity_vector /= vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(0, identity_vector[i]);
  }
  identity_vector = Vector<int, 3>(1);
  vector /= vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector[i], identity_vector[i]);
  }
  std::array<int, 3>values = createRandomWithoutZeroesArray();
  vector = Vector<int, 3>(values);
  vector /=  identity_vector;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector[i], values[i]);
  }
}
struct EqualityOperation: public :: testing::Test
{
  const int dimension = 3;
  std::array<int, 3> values;
  Vector<int, 3> vector_one;
  Vector<int, 3> vector_two;
  void SetUp()
  {
    values = createRandomArray();
    vector_one = Vector<int, 3>(values);
    vector_two = Vector<int, 3>(values);
  }
};

TEST_F(EqualityOperation, handlesEqualityIdentity)
{
  Vector<int, 3> identiy = Vector<int, 3>();
  Vector<int, 3> vector_two = vector_one - identiy;
  ASSERT_TRUE(vector_one == vector_two);
  vector_two = vector_one - vector_one;
  ASSERT_TRUE(vector_two == identiy);
}
TEST_F(EqualityOperation, handlesEquality)
{
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_one[i], vector_two[i]);
  }
  ASSERT_TRUE(vector_one == vector_two);
}
TEST_F(EqualityOperation, handlesSymmetry)
{
  ASSERT_TRUE(vector_one == vector_two);
  ASSERT_TRUE(vector_two == vector_one);
}
}
}
}
