/*
 * Author: Pablo Morandé & Flori
 * Organisation: HYPED
 * Date: 13/11/2020
 * Description: Testing file for data.cpp
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
#include "gtest/gtest.h"
#include "utils/math/vector.hpp"


namespace hyped
{
namespace utils
{
namespace math
{

std::array<int, 3> createRandomArray()
{
  std::array<int, 3> output = std::array<int, 3>();
  for (int i = 0;i < 3;i++) {
    output[i] = rand()%1000;
  }
  return output;
}
template<int dimension>
void fillRandomArray(std::array<int, dimension>&a)
{
  for (int i = 0;i < dimension;i++) {
    a[i] = rand();
  }
}
TEST(ConstructorTest, handlesZeroArgumentConstructor)
{
  const int dimension = 3;
  Vector<int, dimension> example = Vector<int, dimension>();
  ASSERT_EQ(0, example.norm());
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(0, example[i]);
  }
}
TEST(ConstructorTest, handlesConstantConstructor)
{
  const int element = 2;
  const int dimension = 3;
  Vector<int, dimension> example = Vector<int, dimension>(element);
  ASSERT_EQ(element*std::pow(dimension, 0.5), example.norm());
}
TEST(ConstructorTest, handlesArrayContructor)
{
  const int dimension = 3;
  std::array<int, dimension> array =std::array<int, dimension>();
  array.fill(10);
  Vector<int, dimension> vector = Vector<int, dimension>(array);
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(array[i], vector[i]);
  }
}
TEST(ConstructorTest, handlesArrayContructorEmptyListTest)
{
  const int dimension = 3;
  std::array<int, dimension> array =std::array<int, dimension>();
  Vector<int, dimension> vector = Vector<int, dimension>(array);
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(array[i], vector[i]);
  }
}
TEST(ConstructorTest, handlesListConstructor)
{
  const int dimension = 3;
  std::initializer_list<int> list = std::initializer_list<int>({1, 2, 3});
  Vector<int, dimension> vector = Vector<int, dimension>(list);
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(*(list.begin()+i), vector[i]);
  }
}
TEST(ConstructorTest, handlesDifferentTypeVector)
{
  const int dimension = 3;
  std::array<float, dimension> array_one =std::array<float, dimension>();
  array_one.fill(10.1);
  Vector<float, dimension> vector_one = Vector<float, dimension>(array_one);
  std::array<int, dimension> array_two =std::array<int, dimension>();
  array_two.fill(static_cast<int>(10.1));
  Vector<int, dimension> vector_two = Vector<int, dimension>(vector_one);
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(array_two[i], vector_two[i]);
  }
}
TEST(OperationsTest, allowsAccessAndModification)
{
  const int dimension = 3;
  const int element = 10;
  const int new_element = 11;
  Vector<int, dimension> vector_one = Vector<int, dimension>(element);
  float norm_one = vector_one.norm();
  vector_one[0] = new_element;
  float norm_new = vector_one.norm();
  ASSERT_NE(norm_one, norm_new);
  ASSERT_NE(vector_one[0], 10);
  ASSERT_EQ(vector_one[0], 11);
}
TEST(OperationsTest, allowsAdding)
{
  const int dimension = 3;
  std::array<int, dimension> array_one = createRandomArray();
  Vector<int, dimension> vector_one = Vector<int, dimension>(array_one);
  std::array<int, dimension> array_two = createRandomArray();
  Vector<int, dimension> vector_two = Vector<int, dimension>(array_two);
  vector_one+=vector_two;
  for (int i = 0;i <dimension;i++) {
    ASSERT_EQ(vector_one[i], vector_two[i]+array_one[i]);
  }
}
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
    vector_one = Vector<int, 3>(createRandomArray());
    vector_two = Vector<int, 3>(createRandomArray());
    vector_three = Vector<int, 3>(createRandomArray());
    vector_result_one = Vector<int, 3>();
    vector_result_two = Vector<int, 3>();
  }
};
TEST_F(OperationsByConstant, handlesAutoAdditionWithConstant)
{
  vector_result_one = vector_one + kValue;
  vector_result_two = kValue + vector_one;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], vector_one[i] + kValue);
    ASSERT_EQ(vector_result_two[i], vector_one[i] + kValue);
  }
}
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
TEST_F(OperationsByConstant, handlesAutoSubstractionWithConstant)
{
  vector_result_one = vector_one - kValue;
  vector_result_two = kValue - vector_one;
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_result_one[i], vector_one[i] - kValue);
    ASSERT_EQ(vector_result_two[i], -vector_one[i] + kValue);
  }
}
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
TEST_F(Associativity, isAutoAdditionAssociative)
{
  vector_result_one = (vector_one+ vector_two) + vector_three;
  vector_result_two = vector_one + (vector_two+vector_three);
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(vector_result_two[i], vector_result_one[i]);
  }
}
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
TEST_F(Associativity, isAutoMultiplicationAssociative)
{
  vector_result_one = (vector_one * vector_two) * vector_three;
  vector_result_two = vector_one * (vector_two * vector_three);
  for (int i = 0;i < dimension;i++) {
    ASSERT_EQ(vector_result_two[i], vector_result_one[i]);
  }
}
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
TEST_F(Commutativity, isAutoAdditionCommutative)
{
  vector_result_one = vector_two + vector_one;
  vector_result_two = vector_one + vector_two;
  for (int i = 0;i < dimension;i++) {
  ASSERT_EQ(vector_result_one[i], vector_result_two[i]);
  }
}
TEST_F(Commutativity, isAutoSubstractionNotCommutative)
{
  vector_result_one = vector_two - vector_one;
  vector_result_two = vector_one - vector_two;
  for (int i = 0;i < dimension;i++) {
  ASSERT_EQ(vector_result_one[i], -vector_result_two[i]);
  }
}
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
TEST_F(Commutativity, isAutoMultiplicationCommutative)
{
  vector_result_one = vector_two * vector_one;
  vector_result_two = vector_one * vector_two;
  for (int i = 0;i < dimension;i++) {
  ASSERT_EQ(vector_result_one[i], vector_result_two[i]);
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
