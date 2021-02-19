/*
 * Author: Callum Alexander
 * Organisation: HYPED
 * Date: 24/01/2021
 * Description: Testing file for quaternion.hpp
 *
 *    Copyright 2021 HYPED
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
#include "utils/math/quaternion.hpp"
#include "utils/math/vector.hpp"
#include "utils/system.hpp"
#include "data/data_point.hpp"


namespace hyped {
namespace utils {
namespace math {

// -------------------------------------------------------------------------------------------------
// Helper Functions
// -------------------------------------------------------------------------------------------------

/**
 * @brief Helper function used to create an array of 4 random
 * integers. Used when instantiating classes randomly. 4 is the
 * fixed size of the array generated as that is the fixed size of
 * a quaternion instance.
 * @returns array of 4 integers
 */
std::array<int, 4> createRandomArrayForQuaternion()
{
  std::srand(time(0));
  std::array<int, 4> output = std::array<int, 4>();
  for (int i = 0;i < 4;i++) {
    output[i] = rand()%1000;
  }
  return output;
}

/**
 * @brief Function which compares two floats; code taken from:
 * https://www.tutorialspoint.com/floating-point-comparison-in-cplusplus
 * @param elem1 float number to be compared
 * @param elem2 float number to be comddddpared
 * @param epsilon
 * @returns true if floats to be compared are equal
 */
bool compare_float_for_quaternion(float elem1, float elem2, float epsilon = 0.1f)
{
  if (fabs(elem1 - elem2) < epsilon) {
       return true;
  } else {
       return false;
  }
}

/**
 * @brief Function which calculates the norm of a quaternion
 * @param Quaternion instance
 * @returns float containing the computed norm of the quaternion.
 */
float calculateNorm(Quaternion<int> quat)
{
  float sumSquare = 0;
  for (int i = 0; i < 4; i++) {
    sumSquare += pow(quat[i], 2);
  }
  return sqrt(sumSquare);
}

// -------------------------------------------------------------------------------------------------
// Functionality
// -------------------------------------------------------------------------------------------------

/**
 * Struct used for testing the functionality
 * of quaternion.hpp
 */
struct QuaternionFunctionality : public::testing::Test
{
    /* data */
    const int kSizeOfQuaternion = 4;
    Quaternion<int> quaternion_one;
    Quaternion<int> quaternion_two;

    Quaternion<int> quaternion_result_one;
    Quaternion<int> quaternion_result_two;

    Vector<int, 4> test_vector;
    Vector<int, 3> test_vector_3d;

    float arithmetic_result;

    int kValue;

    void SetUp()
    {
        kValue = rand()%1000;
        quaternion_one = Quaternion<int>(rand()%1000, rand()%1000, rand()%1000, rand()%1000);
        quaternion_two = Quaternion<int>(rand()%1000, rand()%1000, rand()%1000, rand()%1000);
    }
};

// -------------------------------------------------------------------------------------------------
// Tests
// -------------------------------------------------------------------------------------------------

/**
 * @brief Test to determine the correct instantiation of the zero
 * Quaternion class instance
 */
TEST_F(QuaternionFunctionality, handlesConstructionOfZeroQuaternion)
{
    Quaternion<int> zeroQuaternion = Quaternion<int>(0);
    ASSERT_EQ(0, zeroQuaternion.norm());
    for (int i = 0; i < kSizeOfQuaternion; i++) {
        ASSERT_EQ(0, zeroQuaternion[i]);
    }
}

/**
 * @brief Test to determine the correct instantiation of a non zero
 * Quaternion class instance.
 */
TEST_F(QuaternionFunctionality, handlesConstructionOfNonZeroQuaternion)
{
  int component_array[kSizeOfQuaternion];
  for (int i = 0; i < kSizeOfQuaternion; i++) {
    component_array[i] = rand()%1000;
    while (component_array[i] == 0) {
      component_array[i] = rand()%1000;
    }
  }
  Quaternion<int> nonZeroQuaternion = Quaternion<int>(
    component_array[0],
    component_array[1],
    component_array[2],
    component_array[3]);
  for (int i = 0; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(nonZeroQuaternion[i], component_array[i]);
  }
}

/**
 * @brief Test to determine the correct construction of Quaternion
 * instances by Vector instances. This test includes contruction by
 * a 4D Vector, 3D Vector and constant, and 3D Vector.
 */
TEST_F(QuaternionFunctionality, handlesConstructionByVector)
{
  // Testing construction by 4D Vector
  test_vector = Vector<int, 4>(createRandomArrayForQuaternion());
  Quaternion<int> quaternion_vector = Quaternion<int>(test_vector);
  for (int i = 0; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(test_vector[i], quaternion_vector[i]);
  }

  std::array<int, 3> component_array;
  for (int i = 0; i < 3; i++) {
    component_array[i] = rand()%1000;
    while (component_array[i] == 0) {
      component_array[i] = rand()%1000;
    }
  }
  // Testing  construction by 3D Vector
  test_vector_3d = Vector<int, 3>(component_array);
  Quaternion<int> quaternion_vector_3d = Quaternion<int>(test_vector_3d);
  ASSERT_EQ(0, quaternion_vector_3d[0]);
  for (int i = 1; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(quaternion_vector_3d[i], test_vector_3d[i-1]);
  }

  // Testing construction by 3D Vector and constant
  Quaternion<int> quaternion_vector_3d_const = Quaternion<int>(kValue, test_vector_3d);
  ASSERT_EQ(kValue, quaternion_vector_3d_const[0]);
  for (int i = 1; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(quaternion_vector_3d_const[i], test_vector_3d[i-1]);
  }
}

/**
 * @brief Test to determine whether the Quaternion class supports
 * addition with a scalar constant.
 * The test is performed with randomly instantiated quaternions and
 * random constant value
 */
TEST_F(QuaternionFunctionality, handlesAutoAdditionByConstant)
{
    quaternion_result_one = quaternion_one + kValue;
    quaternion_result_two = kValue + quaternion_one;
    for (int i = 0; i < kSizeOfQuaternion; i++) {
        ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] + kValue);
        ASSERT_EQ(quaternion_result_two[i], quaternion_one[i] + kValue);
    }
}

/**
 * @brief Test to determine whether the Quaternion class supports
 * the incrementation operation on constant values in the normal way
 * (adding a constant to each component of the Quaternion)
 */
TEST_F(QuaternionFunctionality, handlesAdditionByConstant)
{
  quaternion_result_one += kValue;
  for (int i = 0; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(quaternion_result_one[i], kValue);
  }
  quaternion_result_one += quaternion_one;
  for (int i = 0; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] + kValue);
  }
}

/**
 * @brief Test to determine whether the Quaternion class supports
 * subtraction with a scalar constant
 * Test is is performed with randomly instantiated quaternions and
 * random constant values
 */
TEST_F(QuaternionFunctionality, handlesAutoSubtractionByConstant)
{
    quaternion_result_one = quaternion_one - kValue;
    for (int i = 0; i < kSizeOfQuaternion; i++) {
        ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] - kValue);
    }
}

/**
 * @brief Test to determine whether the Quaterion class supports
 * decrementation by a constant value (subtracting a constant
 * from each component)
 */
TEST_F(QuaternionFunctionality, handlesSubtractionByConstant)
{
  quaternion_result_one -= kValue;
  for (int i = 0; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(quaternion_result_one[i], -kValue);
  }
  quaternion_result_one += quaternion_one;
  for (int i = 0; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] - kValue);
  }
}

/**
 * @brief Test to determine whether the Quaternion class suppots
 * auto multiplication by a constant scalar.
 * This test uses a random quaternion instance and constant.
 */
TEST_F(QuaternionFunctionality, handleAutoMultiplicationByConstant)
{
  quaternion_result_one = quaternion_one * kValue;
  quaternion_result_two = kValue * quaternion_one;
  for (int i = 0; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] * kValue);
    ASSERT_NE(quaternion_result_two[i], quaternion_one[i] * kValue);
  }
}

/**
 * @brief Test to determine whether the Quaternion class supports
 * multiplication by a constant scalar using the (*=) operator.
 * This test uses a random quaterion instance and constant.
 */
TEST_F(QuaternionFunctionality, handleMultiplicationByConstant)
{
  quaternion_result_one = quaternion_one;
  quaternion_result_one *= kValue;
  for (int i = 0; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] * kValue);
  }
}
/**
 * @brief Test to determine whether the Quaternion class supports
 * division by a constant scalar
 * Test uses random quaternion instance and scalar value
 */
TEST_F(QuaternionFunctionality, handleAutoDivisionByConstant)
{
  quaternion_result_one = quaternion_one / kValue;
  for (int i = 0; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] / kValue);
  }
}

/**
 * @brief Test to determine whether the Quaternion class supports
 * division by a constant scalar using the (/=) operator.
 * Test uses random quaternion instance and constant.
 */
TEST_F(QuaternionFunctionality, handleDivisionByConstant)
{
  quaternion_result_one = quaternion_one;
  quaternion_result_one /= kValue;
  for (int i = 0; i < kSizeOfQuaternion; i++) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] / kValue);
  }
}

/**
 * @brief Test to determine whether the Quaternion class supports the
 * multiplication of two quaternions.
 * This test uses two randomly generated quaternions objects.
 */
TEST_F(QuaternionFunctionality, handleQuaternionMultiplicationByQuaternion)
{
  quaternion_result_one = quaternion_one;
  quaternion_result_one *= quaternion_two;
  // Asserting correct multiplication for first element
  ASSERT_EQ(quaternion_result_one[0],
            quaternion_two[0]*quaternion_one[0] - quaternion_two[1]*quaternion_one[1]
            - quaternion_two[2]*quaternion_one[2] - quaternion_two[3]*quaternion_one[3]);
  // Asserting correct multiplication for second element
  ASSERT_EQ(quaternion_result_one[1],
            quaternion_two[0]*quaternion_one[1] + quaternion_two[1]*quaternion_one[0]
            - quaternion_two[2]*quaternion_one[3] + quaternion_two[3]*quaternion_one[2]);
  // Asserting correct multiplication for third element
  ASSERT_EQ(quaternion_result_one[2],
            quaternion_two[0]*quaternion_one[2] + quaternion_two[1]*quaternion_one[3]
            + quaternion_two[2]*quaternion_one[0] - quaternion_two[3]*quaternion_one[1]);
  // Asserting correct multiplication for forth element
  ASSERT_EQ(quaternion_result_one[3],
            quaternion_two[0]*quaternion_one[3] - quaternion_two[1]*quaternion_one[2]
            + quaternion_two[2]*quaternion_one[1] + quaternion_two[3]*quaternion_one[0]);
}

/**
 * @brief Test to determine whether the correct norm (magnitude) of the Quaternion
 * is calculated correctly. This test uses a compare float function to assert correctness
 * to one decimal place before asserting true.
 * This test uses two randomly generated Quaternion instances.
 */
TEST_F(QuaternionFunctionality, handleQuaternionNormCalculation)
{
  ASSERT_TRUE(compare_float_for_quaternion(quaternion_one.norm(), calculateNorm(quaternion_one)));
  ASSERT_TRUE(compare_float_for_quaternion(quaternion_two.norm(), calculateNorm(quaternion_two)));
}

}}}  // hyped::utils::math
