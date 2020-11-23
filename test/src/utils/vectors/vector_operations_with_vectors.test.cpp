#include <math.h>
#include <cstdlib>
#include "gtest/gtest.h"
#include "utils/math/vector.hpp"
#include "math.h"
#include "ctime"
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
 * @brief Helper method used to generate an array of three random integers.
 * each integer will be in the range -1000, 1000 (not inclusive)
 */
std::array<int, 3> createRandomArray()
{
  std::srand(time(0));
  std::array<int, 3> output = std::array<int, 3>();
  for (int i = 0;i < 3;i++) {
    output[i] = rand()%1000;
  }
  return output;
}
/**
 * @brief Helper method used to generate an array of three random integers.
 * Also none of those Integers will be zero.
 * each integer will be in the range -1000, 1000 (not inclusive)
 */
std::array<int, 3> createRandomWithoutZeroesArray()
{
  std::array<int, 3> output = std::array<int, 3>();
  for (int i = 0;i < 3;i++) {
    output[i] = rand()%1000;
    while (output[i] == 0) {
      output[i] = rand()%1000;
    }
  }
  return output;
}
/**
 * @brief Struct used to set up all the variables (vectors) used in the tests for basic operations
 * of vector with other vectors.
 * This tests will check if that basic operations are performed the way that they should. However,
 * they do not test properties.
 */
struct OperationsTest : public::testing::Test
{
  const int dimension = 3;
  std::array<int, 3> array_one;
  std::array<int, 3> array_two;
  Vector<int, 3> vector_one;
  Vector<int, 3> vector_two;
  void SetUp()
{
    array_one = createRandomArray();
    array_two = createRandomWithoutZeroesArray();
    vector_one = Vector<int, 3>(array_one);
    vector_two = Vector<int, 3>(array_two);
  }
};
/**
 * @brief Test used to determine if the current implementation of the class vector allows accessing
 * and modifying the elements of a vector in a normal way using [] operator.
 */
TEST_F(OperationsTest, allowsAccessAndModification)
{
  const int element = 10;
  const int new_element = 11;
  vector_one = Vector<int, 3>(element);
  float norm_one = vector_one.norm();
  vector_one[0] = new_element;
  float norm_new = vector_one.norm();
  ASSERT_NE(norm_one, norm_new);
  ASSERT_NE(vector_one[0], 10);
  ASSERT_EQ(vector_one[0], 11);
}
/**
 * @brief Test used to determine if the current implementation of the class vector allows adding  a
 * vector to another vector in place in a normal way (element by element),
 * this tests the operator (+=).
 * The test will be performed using two random 3D vectors.
 */
TEST_F(OperationsTest, allowsAddition)
{
  vector_one+=vector_two;
  for (int i = 0;i <dimension;i++) {
    ASSERT_EQ(vector_one[i], vector_two[i]+array_one[i]);
  }
}
/**
 * @brief Test used to determine if the current implementation of the class vector allows adding in
 * a normal way (element by element), this tests the operator (+).
 * The test will be performed using two random 3D vectors.
 */
TEST_F(OperationsTest, allowsAutoAddition)
{
  vector_one = vector_one + vector_two;
  for (int i = 0;i <dimension;i++) {
    ASSERT_EQ(vector_one[i], vector_two[i] + array_one[i]);
  }
}
/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * substracting a vector to another vector in place in a normal way (element by element),
 * this tests the operator (-=).
 * The test will be performed using two random 3D vectors.
 */
TEST_F(OperationsTest, allowsSubstraction)
{
  vector_one-=vector_two;
  for (int i = 0;i <dimension;i++) {
    ASSERT_EQ(vector_one[i], -vector_two[i] + array_one[i]);
  }
}
/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * substracting a vector to another vector in place in a normal way (element by element),
 * this tests the operator (-).
 * The test will be performed using two random 3D vectors.
 */
TEST_F(OperationsTest, allowsAutoSubstraction)
{
  vector_one = vector_one - vector_two;
  for (int i = 0;i <dimension;i++) {
    ASSERT_EQ(vector_one[i], -vector_two[i] + array_one[i]);
  }
}
/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * multipliying a vector to another vector in place in a normal way (element by element),
 * this tests the operator (*=).
 * The test will be performed using two random 3D vectors.
 */
TEST_F(OperationsTest, allowsMultiplication)
{
  vector_one*=vector_two;
  for (int i = 0;i <dimension;i++) {
    ASSERT_EQ(vector_one[i], vector_two[i]*array_one[i]);
  }
}
/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * multipliying a vector to another vector in place in a normal way (element by element),
 * this tests the operator (*).
 * The test will be performed using two random 3D vectors.
 */
TEST_F(OperationsTest, allowsAutoMultiplication)
{
  vector_one = vector_one * vector_two;
  for (int i = 0;i <dimension;i++) {
    ASSERT_EQ(vector_one[i], vector_two[i]*array_one[i]);
  }
}
/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * dividing a vector to another vector in a normal way (element by element),
 * this tests the operator (/).
 * The test will be performed using two random 3D vectors.
 */
TEST_F(OperationsTest, allowsAutoDivision)
{
  vector_one = vector_one / vector_two;
  for (int i = 0;i <dimension;i++) {
    ASSERT_EQ(vector_one[i], static_cast<int>(array_one[i] / vector_two[i]));
  }
}
/**
 * @brief Test used to determine if the current implementation of the class vector allows
 * dividing a vector to another vector in place in a normal way (element by element),
 * this tests the operator (/=).
 * The test will be performed using two random 3D vectors.
 */
TEST_F(OperationsTest, allowsDivision)
{
  vector_one/=vector_two;
  for (int i = 0;i <dimension;i++) {
    ASSERT_EQ(vector_one[i], static_cast<int>(array_one[i]/vector_two[i]));
  }
}
TEST_F(OperationsTest, allowsSqrt)
{
  Vector<int, 3> result_one = vector_one.sqrt();
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(static_cast<int>(std::sqrt(vector_one[i])), result_one[i]);
  }
}
TEST_F(OperationsTest, allowsNorm)
{
  double norm = vector_one.norm();
  double result = 0;
  for (int i = 0; i < dimension; i++) {
    result += vector_one[i] * vector_one[i];
  }
  result = std::sqrt(result);
  ASSERT_EQ(result, norm);
}
TEST_F(OperationsTest, allowsToUnitVector)
{
  Vector<int, 3> result_one = vector_one.toUnitVector();
  double norm = vector_one.norm();
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(result_one[i], static_cast<int>(vector_one[i]/norm));
  }
}
/**
 * @brief Struct used to set up all the variables used in the test the equality operation of vectors
 */
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
/**
 * @brief Test used to some equality properties with the identiy vector (0,0,0)
 * It checks that a vetor minus the identity is equal to the original vector.
 * It also checks that if we substract the vector to itself the the result is equal to the identity.
 */
TEST_F(EqualityOperation, handlesEqualityIdentity)
{
  Vector<int, 3> identiy = Vector<int, 3>();
  Vector<int, 3> vector_two = vector_one - identiy;
  ASSERT_TRUE(vector_one == vector_two);
  vector_two = vector_one - vector_one;
  ASSERT_TRUE(vector_two == identiy);
}
/**
 * @brief Test used to check if the Equality is implemented in a correct way.
 */
TEST_F(EqualityOperation, handlesEquality)
{
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(vector_one[i], vector_two[i]);
  }
  ASSERT_TRUE(vector_one == vector_two);
}
/**
 * @brief Test used to check if the Equality is symmetric.
 */
TEST_F(EqualityOperation, handlesSymmetry)
{
  ASSERT_TRUE(vector_one == vector_two);
  ASSERT_TRUE(vector_two == vector_one);
}
}
}
}
