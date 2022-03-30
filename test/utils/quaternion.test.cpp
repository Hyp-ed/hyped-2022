#include <math.h>

#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include <data/data_point.hpp>
#include <utils/math/quaternion.hpp>
#include <utils/math/vector.hpp>
#include <utils/system.hpp>

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
  for (size_t i = 0; i < 4; ++i) {
    output[i] = rand() % 1000;
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
  for (size_t i = 0; i < 4; ++i) {
    sumSquare += pow(quat[i], 2);
  }
  return sqrt(sumSquare);
}

/**
 * @brief Helper function used to create an array of 3 random
 * integers. Used when instantiating classes randomly. 3 is the
 * fixed size of the array generated as that is the fixed size of
 * a vector instance that is used in the tests.
 * @returns array of 3 integers
 */
std::array<int, 3> createRandomArrayForVector()
{
  std::srand(time(0));
  std::array<int, 3> output = std::array<int, 3>();
  for (size_t i = 0; i < 3; ++i) {
    output[i] = rand() % 1000;
  }
  return output;
}

// -------------------------------------------------------------------------------------------------
// Functionality
// -------------------------------------------------------------------------------------------------

/**
 * Class used for testing the construction
 * of quaternion.hpp
 */
class QuaternionConstruction : public ::testing::Test {
 protected:
  /* data */
  Vector<int, 4> test_vector;
  Vector<int, 3> test_vector_3d;

  int kValue;

  std::string zero_construction_error   = "Construction of zero quaternion handled incorrectly";
  std::string nzero_construction_error  = "Construction of std quaternion handled incorrectly";
  std::string vector_construction_error = "Construction of vector quaternion incorrect";

  void SetUp()
  {
    std::srand(time(0));
    kValue = rand() % 1000 + 1;
  }
};

/**
 * Class used for testing the functionality
 * of quaternion.hpp
 */
class QuaternionFunctionality : public ::testing::Test {
 protected:
  /* data */
  Quaternion<int> quaternion_one;
  Quaternion<int> quaternion_two;

  Quaternion<int> quaternion_result_one;
  Quaternion<int> quaternion_result_two;

  float arithmetic_result;

  int kValue;

  // Error messages
  std::string autoaddition_error    = "Autoaddition by constant incorrect";
  std::string addition_error        = "Addition by constant incorrect";
  std::string autosubtraction_error = "Autosubtraction by constant incorrect";
  std::string subtraction_error     = "Subtraction by constant incorrect";
  std::string autoproduct_error     = "Automultiplication by constant incorrect";
  std::string product_error         = "Multiplication by constant incorrect";
  std::string autodivision_error    = "Autodivision by constant incorrect";
  std::string division_error        = "Division by constant incorrect";
  std::string quaternion_mult_error = "Quaternion multiplication incorrect";
  std::string norm_error            = "Calculation of quaternion norm incorrect";

  void SetUp()
  {
    std::srand(time(0));
    kValue         = rand() % 1000 + 1;
    quaternion_one = Quaternion<int>(rand() % 1000, rand() % 1000, rand() % 1000, rand() % 1000);
    quaternion_two = Quaternion<int>(rand() % 1000, rand() % 1000, rand() % 1000, rand() % 1000);
  }
};

// -------------------------------------------------------------------------------------------------
// Tests
// -------------------------------------------------------------------------------------------------

/**
 * @brief Test to determine the correct instantiation of the zero
 * Quaternion class instance
 */
TEST_F(QuaternionConstruction, handlesConstructionOfZeroQuaternion)
{
  Quaternion<int> zeroQuaternion = Quaternion<int>(0);
  ASSERT_EQ(0, zeroQuaternion.norm()) << zero_construction_error;
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(0, zeroQuaternion[i]);
  }
}

/**
 * @brief Test to determine the correct instantiation of a non zero
 * Quaternion class instance.
 */
TEST_F(QuaternionConstruction, handlesConstructionOfNonZeroQuaternion)
{
  int component_array[4];
  for (size_t i = 0; i < 4; ++i) {
    component_array[i] = rand() % 1000;
    while (component_array[i] == 0) {
      component_array[i] = rand() % 1000;
    }
  }
  Quaternion<int> nonZeroQuaternion = Quaternion<int>(component_array[0], component_array[1],
                                                      component_array[2], component_array[3]);
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(nonZeroQuaternion[i], component_array[i]) << nzero_construction_error;
  }
}

/**
 * @brief Test to determine the correct construction of Quaternion
 * instances by Vector instances. This test includes contruction by
 * a 4D Vector, 3D Vector and constant, and 3D Vector.
 */
TEST_F(QuaternionConstruction, handlesConstructionByVector)
{
  // Testing construction by 4D Vector
  test_vector                       = Vector<int, 4>(createRandomArrayForQuaternion());
  Quaternion<int> quaternion_vector = Quaternion<int>(test_vector);
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(test_vector[i], quaternion_vector[i]) << vector_construction_error;
  }

  // Testing  construction by 3D Vector
  test_vector_3d                       = Vector<int, 3>(createRandomArrayForVector());
  Quaternion<int> quaternion_vector_3d = Quaternion<int>(test_vector_3d);
  ASSERT_EQ(0, quaternion_vector_3d[0]) << vector_construction_error;
  for (size_t i = 1; i < 4; ++i) {
    ASSERT_EQ(quaternion_vector_3d[i], test_vector_3d[i - 1]) << vector_construction_error;
  }

  // Testing construction by 3D Vector and constant
  Quaternion<int> quaternion_vector_3d_const = Quaternion<int>(kValue, test_vector_3d);
  ASSERT_EQ(kValue, quaternion_vector_3d_const[0]) << vector_construction_error;
  for (size_t i = 1; i < 4; ++i) {
    ASSERT_EQ(quaternion_vector_3d_const[i], test_vector_3d[i - 1]) << vector_construction_error;
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
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] + kValue) << autoaddition_error;
    ASSERT_EQ(quaternion_result_two[i], quaternion_one[i] + kValue) << autoaddition_error;
  }
}

/**
 * @brief Test to determine whether the Quaternion class supports
 * the incrementation operation on constant values in the normal way
 * (adding a constant to each component of the Quaternion)
 */
TEST_F(QuaternionFunctionality, handlesAdditionByConstant)
{
  Quaternion<int> quaternion_identity = Quaternion<int>(0);
  quaternion_identity += kValue;
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(quaternion_identity[i], kValue) << addition_error;
  }
  quaternion_result_one = quaternion_one;
  quaternion_result_one += kValue;
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] + kValue) << addition_error;
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
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] - kValue) << autosubtraction_error;
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
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(quaternion_result_one[i], -kValue) << subtraction_error;
  }
  quaternion_result_one += quaternion_one;
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] - kValue) << subtraction_error;
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
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] * kValue) << autoproduct_error;
    ASSERT_NE(quaternion_result_two[i], quaternion_one[i] * kValue) << autoproduct_error;
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
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] * kValue) << product_error;
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
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] / kValue) << autodivision_error;
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
  for (size_t i = 0; i < 4; ++i) {
    ASSERT_EQ(quaternion_result_one[i], quaternion_one[i] / kValue) << division_error;
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
            quaternion_two[0] * quaternion_one[0] - quaternion_two[1] * quaternion_one[1]
              - quaternion_two[2] * quaternion_one[2] - quaternion_two[3] * quaternion_one[3])
    << quaternion_mult_error;
  // Asserting correct multiplication for second element
  ASSERT_EQ(quaternion_result_one[1],
            quaternion_two[0] * quaternion_one[1] + quaternion_two[1] * quaternion_one[0]
              - quaternion_two[2] * quaternion_one[3] + quaternion_two[3] * quaternion_one[2])
    << quaternion_mult_error;
  // Asserting correct multiplication for third element
  ASSERT_EQ(quaternion_result_one[2],
            quaternion_two[0] * quaternion_one[2] + quaternion_two[1] * quaternion_one[3]
              + quaternion_two[2] * quaternion_one[0] - quaternion_two[3] * quaternion_one[1])
    << quaternion_mult_error;
  // Asserting correct multiplication for forth element
  ASSERT_EQ(quaternion_result_one[3],
            quaternion_two[0] * quaternion_one[3] - quaternion_two[1] * quaternion_one[2]
              + quaternion_two[2] * quaternion_one[1] + quaternion_two[3] * quaternion_one[0])
    << quaternion_mult_error;
}

/**
 * @brief Test to determine whether the correct norm (magnitude) of the Quaternion
 * is calculated correctly. This test uses a compare float function to assert correctness
 * to one decimal place before asserting true.
 * This test uses two randomly generated Quaternion instances.
 */
TEST_F(QuaternionFunctionality, handleQuaternionNormCalculation)
{
  ASSERT_TRUE(compare_float_for_quaternion(quaternion_one.norm(), calculateNorm(quaternion_one)))
    << norm_error;
  ASSERT_TRUE(compare_float_for_quaternion(quaternion_two.norm(), calculateNorm(quaternion_two)))
    << norm_error;
}

}  // namespace math
}  // namespace utils
}  // namespace hyped
