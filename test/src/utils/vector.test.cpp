#include "gtest/gtest.h"
#include "utils/math/vector.hpp"
#include <math.h>
#include <cstdlib>

namespace hyped{
namespace utils{
namespace math{

std::array<int,3> createRandomArray(){
  std::array<int,3> output = std::array<int,3>();
  for(int i = 0;i<3;i++){
    output[i] = rand();
  }

  return output;

}

template<int dimension>
void fillRandomArray(std::array<int,dimension>&a){
  for(int i = 0;i<dimension;i++){
    a[i] = rand();
  }
  


}
TEST(ConstructorTest,handlesZeroArgumentConstructor){

  const int dimension = 3;
  Vector<int,dimension> example = Vector<int,dimension>();
  ASSERT_EQ(0,example.norm());
  for(int i = 0;i <dimension;i++){
    ASSERT_EQ(0,example[i]);
  }

}
TEST(ConstructorTest,handlesConstantConstructor){
  const int element = 2;
  const int dimension = 3;
  Vector<int,dimension> example = Vector<int,dimension>(element);
  ASSERT_EQ(element*std::pow(dimension,0.5),example.norm());

}
TEST(ConstructorTest,handlesArrayContructor){
  const int dimension = 3;
  std::array<int ,dimension> array =std::array<int ,dimension>();
  array.fill(10);
  Vector<int,dimension> vector = Vector<int,dimension>(array);
  for(int i = 0;i <dimension;i++){
    ASSERT_EQ(array[i],vector[i]);
  }
}
TEST(ConstructorTest,handlesArrayContructorEmptyListTest){
  const int dimension = 3;
  std::array<int ,dimension> array =std::array<int ,dimension>();
  Vector<int,dimension> vector = Vector<int,dimension>(array);
  for(int i = 0;i <dimension;i++){
    ASSERT_EQ(array[i],vector[i]);
  }
}
TEST(ConstructorTest,handlesListConstructor){
  const int dimension = 3;
  std::initializer_list<int> list = std::initializer_list<int>({1,2,3});
  Vector<int,dimension> vector = Vector<int,dimension>(list);
  for(int i = 0;i <dimension;i++){
    ASSERT_EQ(*(list.begin()+i),vector[i]);
  }
}
TEST(ConstructorTest,handlesDifferentTypeVector){
  const int dimension = 3;
  std::array<float ,dimension> array_one =std::array<float ,dimension>();
  array_one.fill(10.1);
  Vector<float,dimension> vector_one = Vector<float,dimension>(array_one);
  std::array<int ,dimension> array_two =std::array<int ,dimension>();
  array_two.fill(int(10.1));
  Vector<int,dimension> vector_two = Vector<int,dimension>(vector_one);
  for(int i = 0;i <dimension;i++){
    ASSERT_EQ(array_two[i],vector_two[i]);
  }
}
TEST(OperationsTest,allowsAccessAndModification){
  const int dimension = 3;
  const int element = 10;
  const int new_element = 11;
  Vector<int,dimension> vector_one = Vector<int,dimension>(element);
  float norm_one = vector_one.norm();
  vector_one[0] = new_element;
  float norm_new = vector_one.norm();
  ASSERT_NE(norm_one,norm_new);
  ASSERT_NE(vector_one[0],10);
  ASSERT_EQ(vector_one[0],11);
}
TEST(OperationsTest,allowsAdding){
  const int dimension = 3;
  std::array<int,dimension> array_one=createRandomArray();
  
  Vector<int,dimension> vector_one = Vector<int,dimension>(array_one);
  std::array<int,dimension> array_two=createRandomArray();

  Vector<int,dimension> vector_two= Vector<int,dimension>(array_two);
  vector_one+=vector_two;
  for(int i = 0;i <dimension;i++){
    ASSERT_EQ(vector_one[i],vector_two[i]+array_one[i]);
  }
}
struct Associativity : public::testing::Test{
  const int dimension = 3;
  Vector<int,3> vector_one;
  Vector<int,3> vector_two;
  Vector<int,3> vector_three;
  Vector<int,3> vector_result_one;
  Vector<int,3> vector_result_two;
  void SetUp(){
    vector_one= Vector<int,3>(createRandomArray());
    vector_two= Vector<int,3>(createRandomArray());
    vector_three= Vector<int,3>(createRandomArray());
    vector_result_one= Vector<int,3>();
    vector_result_two= Vector<int,3>();

  }
};

TEST_F(Associativity,isAdditionAssociative){
  vector_result_one+=vector_one;
  vector_result_one+= vector_two;
  vector_result_one+=vector_three;
  vector_result_two+=vector_two;
  vector_result_two+=vector_three;
  vector_result_two+=vector_one;
  for(int i = 0;i <dimension;i++){
    ASSERT_EQ(vector_result_two[i],vector_result_one[i]);
  }
}
TEST_F(Associativity,isAutoAdditionAssociative){
  vector_result_one = (vector_one+ vector_two) + vector_three;
  vector_result_two = vector_one + (vector_two+vector_three);
  for(int i = 0;i <dimension;i++){
    ASSERT_EQ(vector_result_two[i],vector_result_one[i]);
  }
}
struct Commutativity : public ::testing::Test {
  const int dimension = 3;
  Vector<int,3> vector_one;
  Vector<int,3> vector_two;
  Vector<int,3> vector_result_one;
  Vector<int,3> vector_result_two;
  void SetUp(){
    vector_one = Vector<int,3>(createRandomArray());
    vector_two = Vector<int,3>(createRandomArray());
    vector_result_one= Vector<int,3>();
    vector_result_two= Vector<int,3>();
  }
};
TEST_F(Commutativity,isAdditionCommutative){
  vector_result_one+=vector_one;
  vector_result_one+= vector_two;
  vector_result_two+=vector_two;
  vector_result_two+=vector_one;
  for(int i = 0;i <dimension;i++){
    ASSERT_EQ(vector_result_two[i],vector_result_one[i]);
  }
}
TEST_F(Commutativity,isSubstractionNotCommutative){
  vector_result_one+=vector_one;
  vector_result_one-= vector_two;
  vector_result_two+=vector_two;
  vector_result_two-=vector_one;
  for(int i = 0;i <dimension;i++){
    ASSERT_EQ(vector_result_two[i],-vector_result_one[i]);
  }
}
TEST_F(Commutativity,isAutoAdditionCommutative){
  vector_result_one = vector_two+vector_one;
  vector_result_two = vector_one+vector_two;
   for(int i = 0;i<dimension;i++){
    ASSERT_EQ(vector_result_one[i],vector_result_two[i]);
  }

}
TEST_F(Commutativity,isAutoSubstractionNotCommutative){
  vector_result_one = vector_two-vector_one;
  vector_result_two = vector_one-vector_two;
   for(int i = 0;i<dimension;i++){
    ASSERT_EQ(vector_result_one[i],-vector_result_two[i]);
  }
}

struct IdentityOperations : public ::testing::Test {
  std::array<int,3> values = createRandomArray();
  const int dimension = 3;
  Vector<int,3> identity_vector;
  Vector<int,3> vector;
  void SetUp(){
    identity_vector = Vector<int,3>();
    vector = Vector<int,3>(values);
  }
};

TEST_F(IdentityOperations,handlesAdditionIdentity){
  vector+=identity_vector;
  for(int i = 0;i<dimension;i++){
    ASSERT_EQ(vector[i],values[i]);
  }

}
TEST_F(IdentityOperations,handlesSubstractionIdentities){
  vector-=identity_vector;
  for(int i = 0;i<dimension;i++){
    ASSERT_EQ(vector[i],values[i]);
  }
  vector-=vector;
  for(int i = 0;i<dimension;i++){
    ASSERT_EQ(vector[i],identity_vector[i]);
  }
}
TEST_F(IdentityOperations,hadlesAutoAdditonIdentity){
  Vector<int,3> output = vector + identity_vector;
  for(int i = 0; i<dimension;i++){
    ASSERT_EQ(vector[i],output[i]);

  }
}

}}
}