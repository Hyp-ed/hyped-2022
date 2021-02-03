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
 */
std::array<int, 4> createRandomArray()
{
  std::srand(time(0));
  std::array<int, 4> output = std::array<int, 4>();
  for (int i = 0;i < 4;i++) {
    output[i] = rand()%1000;
  }
  return output;
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

    float arithmetic_result;

    int kValue = rand()%1000;

    void SetUp()
    {
        quaternion_one = Quaternion<int>(rand()%1000, rand()%1000, rand()%1000, rand()%1000);
        quaternion_two = Quaternion<int>(rand()%1000, rand()%1000, rand()%1000, rand()%1000);
    }
};

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

TEST_F(QuaternionFunctionality, handlesConstructionFromVector)
{
  std::array<int, 4> component_array = createRandomArray();
  test_vector = Vector<int, 4>(component_array);
  
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
 * Cal's notes
 * Functionality to test:
 * 1. Vector to quaternion conversion
 * 4. Quaternion multiplication
 * 5. Scalar multiplication of quaternion
 * 6. Scalar divion of quaternion]
 * 7. Magnitude of quaternion
 * 8........
*/


}}}  // hyped::utils::math
