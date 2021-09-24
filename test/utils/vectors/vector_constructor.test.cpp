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
  const int dimension            = 3;
  Vector<int, dimension> example = Vector<int, dimension>();
  ASSERT_EQ(0, example.norm());
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(0, example[i]);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector handles the
 * constructor of the form Vector<T,dimension>(T const), this should initialized all the values of
 * the vector to be equal to const.
 */
TEST(ConstructorTest, handlesConstantConstructor)
{
  const int element              = 2;
  const int dimension            = 3;
  Vector<int, dimension> example = Vector<int, dimension>(element);
  ASSERT_EQ(element * std::pow(dimension, 0.5), example.norm());
}

/**
 * @brief Test used to determine if the current implementation of the class vector handles the
 * constructor of the form Vector<T,dimension>(std::array<T,dimension> array),
 * this should assing each element of the vector with its correspondent element in the array
 */
TEST(ConstructorTest, handlesArrayContructor)
{
  const int dimension              = 3;
  std::array<int, dimension> array = std::array<int, dimension>();
  array.fill(10);
  Vector<int, dimension> vector = Vector<int, dimension>(array);
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(array[i], vector[i]);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector handles the
 * constructor of the form Vector<T,dimension>(std::array<T,dimension> array). In this case the
 * elements of the array are not initialize so they should be zero, therefore the elements of the
 * vector should also be zero.
 */
TEST(ConstructorTest, handlesArrayContructorEmptyListTest)
{
  const int dimension              = 3;
  std::array<int, dimension> array = std::array<int, dimension>();
  Vector<int, dimension> vector    = Vector<int, dimension>(array);
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(array[i], vector[i]);
    ASSERT_EQ(0, vector[i]);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector handles the
 * constructor of the form Vector<T,dimension>(std::array<T> list),
 * this should assing each element of the vector with its correspondent element in the list
 */
TEST(ConstructorTest, handlesListConstructor)
{
  const int dimension           = 3;
  Vector<int, dimension> vector = Vector<int, dimension>({-4, 29, 62});
  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(i * 33 - 4, vector[i]);
  }
}

/**
 * @brief Test used to determine if the current implementation of the class vector handles the
 * constructor of the form Vector<T,dimension>(Vector<I,dimension> vector),
 * this should assing each element of the vector with its correspondent element in the other vector,
 * as they might not be of the same type fist the constructor should perform a cast.
 */
TEST(ConstructorTest, handlesDifferentTypeVector)
{
  const int dimension = 3;

  std::array<float, dimension> array_one = std::array<float, dimension>();
  array_one.fill(10.1);
  Vector<float, dimension> vector_one  = Vector<float, dimension>(array_one);
  std::array<int, dimension> array_two = std::array<int, dimension>();
  array_two.fill(static_cast<int>(10.1));
  Vector<int, dimension> vector_two = Vector<int, dimension>(vector_one);

  for (int i = 0; i < dimension; i++) {
    ASSERT_EQ(array_two[i], vector_two[i]);
  }
}
}  // namespace math
}  // namespace utils
}  // namespace hyped
