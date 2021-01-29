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
TEST_F(QuaternionFunctionality, handleConstructionOfZeroQuaternion)
{
    Quaternion<int> zeroQuaternion = Quaternion<int>(0);
    ASSERT_EQ(0, zeroQuaternion.norm());
    for (int i = 0; i < kSizeOfQuaternion; i++) {
        ASSERT_EQ(0, zeroQuaternion[i]);
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
 * @brief Test to determin whether the Quaternion class supports
 * subtraction with a scalar constant
 * Testis is performed with randomly instantiated quaternions and
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
 * Cal's notes
 * Functionality to test:
 * 0. Vector construction testing
 * 1. Vector to quaternion conversion
 * 4. Quaternion multiplication
 * 5. Scalar multiplication of quaternion
 * 6. Scalar divion of quaternion]
 * 7. Magnitude of quaternion
 * 8........
*/


}}}  // hyped::utils::math
