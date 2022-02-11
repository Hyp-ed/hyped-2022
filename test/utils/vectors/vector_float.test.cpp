#include <math.h>

#include <cstdlib>

#include <gtest/gtest.h>

#include <utils/math/vector.hpp>

namespace hyped {
namespace utils {
namespace math {

// -------------------------------------------------------------------------------------------------
// Helper Functions
// -------------------------------------------------------------------------------------------------

/**
 * @brief Function which compares two floats; code taken from:
 * https://www.tutorialspoint.com/floating-point-comparison-in-cplusplus
 * @param elem1 float number to be compared
 * @param elem2 float number to be compared
 * @param epsilon
 * @returns true if floats to be compared are equal
 */
bool compare_float(float elem1, float elem2, float epsilon = 0.1f)
{
  if (fabs(elem1 - elem2) < epsilon) {
    return true;
  } else {
    return false;
  }
}
/**
 * @brief Function which compares two floats; code taken from:
 * https://www.tutorialspoint.com/floating-point-comparison-in-cplusplus
 * @param elem1 float number to be compared
 * @param elem2 float number to be compared
 * @param epsilon
 * @returns true if floats to be compared are equal
 */
bool compare_float_one_decimal(float elem1, float elem2, float epsilon = 0.1f)
{
  if (fabs(elem1 - elem2) < epsilon) {
    return true;
  } else {
    return false;
  }
}

/**
 * @brief generates a random float between two given floats. Code from: https://tinyurl.com/y2phu2q2
 * @param lower Lower bound for randomly generated values
 * @param upper Upper bound for randomly generated values
 * @returns random float between given bounds
 */
float RandomFloatTestingVector(float lower, float upper)
{
  float random = (static_cast<float>(rand())) / static_cast<float>(RAND_MAX);
  float diff   = upper - lower;
  float r      = random * diff;
  return lower + r;
}

/**
 * @brief calculates the magnitude of a vector with elements corresponding with those from argument
 * array
 * @param vector_T array of 100 float values
 * @returns magnitude of vector with corresponding values to array
 */
double CalculateVectorMagnitude(std::array<float, 100> vector_T)
{
  double sum_of_squares = 0;
  for (int i = 0; i < 100; i++) {
    sum_of_squares = sum_of_squares + vector_T[i] * vector_T[i];
  }
  return std::sqrt(sum_of_squares);
}

// -------------------------------------------------------------------------------------------------
// Set Up
// -------------------------------------------------------------------------------------------------

/**
 * @brief Class used for test fixtures checking class functionality and properties for float
 * values.
 */
class VectorFloatTests : public ::testing::Test {
 protected:
  Vector<float, 100> test_vector1_float;
  Vector<float, 100> test_vector2_float;
  Vector<float, 100> test_vector_default = Vector<float, 100>(0);
  float constant                         = RandomFloatTestingVector(0, 1000);
  Vector<float, 100> test_vector_constant;
  Vector<float, 100> test_vector_diff;
  Vector<float, 100> test_vector_sum;
  Vector<float, 100> test_vector_mult;
  Vector<float, 100> test_vector_div;
  Vector<float, 100> test_vector_sqrt;
  Vector<double, 100> unit_vector1;
  Vector<double, 100> unit_vector2;

  int dimension = 100;
  std::array<float, 100> elements1;
  std::array<float, 100> elements2;

  void SetUp()
  {
    std::srand(time(0));
    // Construct two vectors with random elements and their corresponding unit vectors
    for (int i = 0; i < dimension; i++) {
      elements1[i] = RandomFloatTestingVector(0, 1000);
    }
    test_vector1_float = Vector<float, 100>(elements1);

    for (int i = 0; i < dimension; i++) {
      elements2[i] = RandomFloatTestingVector(0, 1000);
    }
    test_vector2_float = Vector<float, 100>(elements2);
    unit_vector2       = test_vector1_float.toUnitVector();
  }

  void TearDown() {}
};

// -------------------------------------------------------------------------------------------------
// Vector Constructor Testing
// -------------------------------------------------------------------------------------------------

/**
 * @brief Tests no-argument constructor and zero-vector properties for a vector with float values.
 */
TEST_F(VectorFloatTests, handlesNoArgumentConstructor)
{
  ASSERT_EQ(0, test_vector_default.norm());
  int ran_index = rand() % 100;
  ASSERT_EQ(0, test_vector_default.operator[](ran_index));

  // Checking scalar operations
  ran_index = rand() % 100;
  int a     = rand() % 1000;
  ASSERT_EQ(0, test_vector_default.operator+=(a).operator-=(a).operator[](ran_index));
  ASSERT_EQ(0, test_vector_default.operator*=(a).operator[](ran_index));

  // Checking vector operations
  ASSERT_EQ(0, test_vector_default.operator*=(test_vector1_float).operator[](ran_index));
  test_vector_default.operator+=(test_vector1_float).operator-=(test_vector1_float);
  ASSERT_EQ(0, test_vector_default.operator[](ran_index));
}

/**
 * @brief Tests constructor taking a constant as an argument - all vector elements should be
 * equal to said constant after constructor is called.
 */
TEST_F(VectorFloatTests, handlesConstantConstructor)
{
  test_vector_constant = Vector<float, 100>(constant);
  int ran_index        = rand() % 100;
  ASSERT_EQ(constant, test_vector_constant.operator[](ran_index));

  // Checking scalar operations
  ran_index  = rand() % 100;
  int a      = rand() % 1000;
  float elem = test_vector_constant.operator+=(a).operator-=(a).operator[](ran_index);
  ASSERT_TRUE(compare_float_one_decimal(constant, elem));
  ran_index     = rand() % 100;
  float elem_eq = test_vector_constant.operator*=(a).operator[](ran_index);
  ASSERT_TRUE(compare_float_one_decimal(constant * a, elem_eq));
}

/**
 * @brief Tests constructor given an array
 */
TEST_F(VectorFloatTests, handlesConstructorArray)
{  // Checking if elmements in array at given position match vector elements
  // at given position
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements1[i], test_vector1_float.operator[](i));
    ASSERT_EQ(elements2[i], test_vector2_float.operator[](i));
  }
}

// -------------------------------------------------------------------------------------------------
// Element-wise operation testing (operations involving two vectors) - includes auto
// -------------------------------------------------------------------------------------------------

/**
 * @brief Tests element-wise difference (operator-=(vector1, vector2))
 */
TEST_F(VectorFloatTests, handlesElemWiseDifference)
{
  test_vector_diff = test_vector2_float.operator-=(test_vector1_float);

  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements2[i] - elements1[i], test_vector_diff.operator[](i));
  }
}

/**
 * @brief Tests element-wise difference (operator-=(vector1, vector2) - auto)
 */
TEST_F(VectorFloatTests, handlesElemWiseDifferenceAuto)
{
  test_vector_diff = operator-(test_vector1_float, test_vector2_float);
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements1[i] - elements2[i], test_vector_diff.operator[](i));
  }
}

/**
 * @brief Tests element-wise addition (operator+=(vector1, vector2))
 */
TEST_F(VectorFloatTests, handlesElementWiseAddition)
{
  test_vector_sum = test_vector2_float.operator+=(test_vector1_float);
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements1[i] + elements2[i], test_vector_sum.operator[](i));
  }
}

/**
 * @brief Tests element-wise addition (operator+=(vector1, vector2) - auto)
 */
TEST_F(VectorFloatTests, handlesElementWiseAdditionAuto)
{
  test_vector_sum = operator+(test_vector1_float, test_vector2_float);
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements1[i] + elements2[i], test_vector_sum.operator[](i));
  }
}

/**
 * @brief Tests element-wise multiplication (operator*=(vector1, vector2))
 */
TEST_F(VectorFloatTests, handlesElemWiseMultiplication)
{
  test_vector_mult = test_vector2_float.operator*=(test_vector1_float);
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements1[i] * elements2[i], test_vector_mult.operator[](i));
  }
}

/**
 * @brief Tests element-wise multiplication (operator*=(vector1, vector2) - auto)
 */
TEST_F(VectorFloatTests, handlesElemWiseMultiplicationAuto)
{
  test_vector_mult = operator*(test_vector1_float, test_vector2_float);
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements1[i] * elements2[i], test_vector_mult.operator[](i));
  }
}

/**
 * @brief Tests element-wise division (operator/=(vector1, vector2))
 */
TEST_F(VectorFloatTests, handlesElemWiseDivision)
{
  test_vector_div = test_vector2_float.operator/=(test_vector1_float);
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements2[i] / elements1[i], test_vector_div.operator[](i));
  }
}

/**
 * @brief Tests element-wise division (auto)
 */
TEST_F(VectorFloatTests, handlesElemWiseDivisionAuto)
{
  test_vector_div = operator/(test_vector2_float, test_vector1_float);
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements2[i] / elements1[i], test_vector_div.operator[](i));
  }
}

// -------------------------------------------------------------------------------------------------
// Constant operations testing (tests operations with a constant)
// -------------------------------------------------------------------------------------------------

/**
 * @brief Tests addition with a constant
 */
TEST_F(VectorFloatTests, handlesConstantAddition)
{
  float float_constant = RandomFloatTestingVector(0, 1000);
  test_vector_sum      = test_vector1_float.operator+=(float_constant);
  test_vector1_float.operator-=(float_constant);
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(test_vector1_float.operator[](i) + float_constant, test_vector_sum.operator[](i));
  }
}

/**
 * @brief Tests substraction by a constant
 */
TEST_F(VectorFloatTests, handlesConstantDifference)
{
  float float_constant = RandomFloatTestingVector(0, 1000);
  test_vector_diff     = test_vector1_float.operator-=(float_constant);
  test_vector1_float.operator+=(float_constant);
  float elem;
  for (int i = 0; i < dimension; i++) {
    elem = test_vector_diff.operator[](i);
    ASSERT_EQ(test_vector1_float.operator[](i) - float_constant, elem);
  }
}

/**
 * @brief Tests scalar multiplication
 */
TEST_F(VectorFloatTests, handlesScalarMultiplication)
{
  float float_constant = RandomFloatTestingVector(0, 1000);
  test_vector_mult     = test_vector1_float.operator*=(float_constant);
  test_vector1_float.operator/=(float_constant);
  float elem;
  for (int i = 0; i < dimension; i++) {
    elem = test_vector_mult.operator[](i);
    ASSERT_EQ(test_vector1_float.operator[](i) * float_constant, elem);
  }
}

/**
 * @brief Tests scalar division
 */
TEST_F(VectorFloatTests, handlesScalarDivision)
{
  float float_constant = RandomFloatTestingVector(0, 1000);
  test_vector_div      = test_vector1_float.operator/=(float_constant);
  test_vector1_float.operator*=(float_constant);
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(test_vector1_float.operator[](i) / float_constant, test_vector_div.operator[](i));
  }
}

// -------------------------------------------------------------------------------------------------
// Testing other Vector class functions
// -------------------------------------------------------------------------------------------------

/**
 * @brief Tests if Vector.sqrt() works as it should - takig the square root of each element
 * in a vector
 */
TEST_F(VectorFloatTests, handlesElementSqrt)
{
  test_vector_sqrt = test_vector2_float.sqrt();
  float elem;
  for (int i = 0; i < dimension; i++) {
    elem = test_vector_sqrt.operator[](i);
    ASSERT_EQ(static_cast<float>(std::sqrt(test_vector2_float[i])), elem);
  }
}

/**
 * @brief Tests calculation of distance between two vectors with float elements.
 */
TEST_F(VectorFloatTests, allowsDistanceBetweenVectors)
{
  std::array<float, 100> vector_diff;
  for (int i = 0; i < dimension; i++) {
    vector_diff[i] = elements2[i] - elements1[i];
  }
  double distance = CalculateVectorMagnitude(vector_diff);

  ASSERT_EQ(distance, test_vector2_float.operator-=(test_vector1_float).norm());
}

/**
 * @brief Tests corectness of unit vector calculation
 */
TEST_F(VectorFloatTests, allowsUnitVectors)
{
  unit_vector1 = test_vector1_float.toUnitVector();
  unit_vector2 = test_vector2_float.toUnitVector();

  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements1[i] / CalculateVectorMagnitude(elements1), unit_vector1.operator[](i));
    ASSERT_EQ(elements2[i] / CalculateVectorMagnitude(elements2), unit_vector2.operator[](i));
  }
  ASSERT_EQ(round(test_vector1_float.toUnitVector().norm()), 1);
}

/**
 * @brief Tests function for returning vector with all elements negated
 */
TEST_F(VectorFloatTests, allowsVectorNegation)
{
  test_vector_diff = test_vector1_float.operator-();
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements1[i] * (-1), test_vector_diff.operator[](i));
  }
  test_vector_diff = test_vector2_float.operator-();
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(elements2[i] * (-1), test_vector_diff.operator[](i));
  }
}

/**
 * @brief Tests if magnitude of vector with float elements is calculated correctly.
 */
TEST_F(VectorFloatTests, handlesVectorMagnitude)
{
  double magnitude_v1 = CalculateVectorMagnitude(elements1);
  double magnitude_v2 = CalculateVectorMagnitude(elements2);
  ASSERT_EQ(magnitude_v1, test_vector1_float.norm());
  ASSERT_EQ(magnitude_v2, test_vector2_float.norm());
}

/**
 * @brief Tests if equality function is behaving as it should
 */
TEST_F(VectorFloatTests, handlesVectorEquality)
{
  test_vector_mult = Vector<float, 100>(test_vector1_float);
  ASSERT_TRUE(test_vector_mult == test_vector1_float);
}
// -------------------------------------------------------------------------------------------------
// Testing vector properties
// -------------------------------------------------------------------------------------------------

/**
 * @brief Tests if unit vector property stating that magnitude = 1 holds
 */
TEST_F(VectorFloatTests, hanldesMagnitudeUnitVector)
{
  unit_vector1 = test_vector1_float.toUnitVector();
  unit_vector2 = test_vector2_float.toUnitVector();
  ASSERT_EQ(1, round(unit_vector1.norm()));
  ASSERT_EQ(1, round(unit_vector2.norm()));
}

/**
 * @brief Tests symmetry of vector equality
 */
TEST_F(VectorFloatTests, handlesVectorEqualitySymmetry)
{
  test_vector_mult = Vector<float, 100>(test_vector1_float);
  ASSERT_TRUE(test_vector_mult == test_vector1_float);
  ASSERT_TRUE(test_vector1_float == test_vector_mult);
}

}  // namespace math
}  // namespace utils
}  // namespace hyped
