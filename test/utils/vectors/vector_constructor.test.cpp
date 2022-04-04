#include <math.h>

#include <cstdlib>

#include <gtest/gtest.h>

#include <utils/math/vector.hpp>

namespace hyped {
namespace utils {
namespace math {

/**
 * @brief Test used to verify that the current implementation of the class vector handles the zero
 * argument constructor in an appropiate way.
 */
TEST(ConstructorTest, handlesZeroArgumentConstructor)
{
  static constexpr size_t kDimension = 3;
  Vector<int, kDimension> example    = Vector<int, kDimension>();
  ASSERT_EQ(0, example.norm());
  for (size_t i = 0; i < kDimension; ++i) {
    ASSERT_EQ(0, example[i]);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector handles the
 * constructor of the form Vector<T,kDimension>(T const), this should initialized all the values of
 * the vector to be equal to const.
 */
TEST(ConstructorTest, handlesConstantConstructor)
{
  const int element                  = 2;
  static constexpr size_t kDimension = 3;
  Vector<int, kDimension> example    = Vector<int, kDimension>(element);
  ASSERT_EQ(element * std::pow(kDimension, 0.5), example.norm());
}

/**
 * @brief Test used to determine if the current implementation of the class vector handles the
 * constructor of the form Vector<T,kDimension>(std::array<T,kDimension> array),
 * this should assing each element of the vector with its correspondent element in the array
 */
TEST(ConstructorTest, handlesArrayContructor)
{
  static constexpr size_t kDimension = 3;
  std::array<int, kDimension> array  = std::array<int, kDimension>();
  array.fill(10);
  Vector<int, kDimension> vector = Vector<int, kDimension>(array);
  for (size_t i = 0; i < kDimension; ++i) {
    ASSERT_EQ(array[i], vector[i]);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector handles the
 * constructor of the form Vector<T,dimension>(std::array<T,kDimension> array). In this case the
 * elements of the array are not initialize so they should be zero, therefore the elements of the
 * vector should also be zero.
 */
TEST(ConstructorTest, handlesArrayContructorEmptyListTest)
{
  static constexpr size_t kDimension = 3;
  std::array<int, kDimension> array  = std::array<int, kDimension>();
  Vector<int, kDimension> vector     = Vector<int, kDimension>(array);
  for (size_t i = 0; i < kDimension; ++i) {
    ASSERT_EQ(array[i], vector[i]);
    ASSERT_EQ(0, vector[i]);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector handles the
 * constructor of the form Vector<T,kDimension>(std::array<T> list),
 * this should assing each element of the vector with its correspondent element in the list
 */
TEST(ConstructorTest, handlesListConstructor)
{
  static constexpr size_t kDimension = 3;
  Vector<int, kDimension> vector     = Vector<int, kDimension>({-4, 29, 62});
  for (size_t i = 0; i < kDimension; ++i) {
    ASSERT_EQ(static_cast<ssize_t>(i) * 33 - 4, vector[i]);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector handles the
 * constructor of the form Vector<T,kDimension>(Vector<I,kDimension> vector),
 * this should assing each element of the vector with its correspondent element in the other vector,
 * as they might not be of the same type fist the constructor should perform a cast.
 */
TEST(ConstructorTest, handlesDifferentTypeVector)
{
  static constexpr size_t kDimension = 3;

  std::array<float, kDimension> array_one = std::array<float, kDimension>();
  array_one.fill(10.1);
  Vector<float, kDimension> vector_one  = Vector<float, kDimension>(array_one);
  std::array<int, kDimension> array_two = std::array<int, kDimension>();
  array_two.fill(static_cast<int>(10.1));
  Vector<int, kDimension> vector_two = Vector<int, kDimension>(vector_one);

  for (size_t i = 0; i < kDimension; ++i) {
    ASSERT_EQ(array_two[i], vector_two[i]);
  }
}
}  // namespace math
}  // namespace utils
}  // namespace hyped
