/*
 * Author: Donald Jennings
 * Organisation: HYPED
 * Date: 06/11/2020
 * Description: Testing file for Differentiator.hpp
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

#include <iostream>
#include "math.h"
#include "gtest/gtest.h"
#include "utils/math/kalman_multivariate.hpp"
#include "utils/system.hpp"

using hyped::utils::math::KalmanMultivariate;

struct KalmanFunctionality : public ::testing::Test {
 protected:
  unsigned int n = 1;
  unsigned int m = 2;
  unsigned int p = 3;
  unsigned int q = 4;

  KalmanMultivariate kalman = KalmanMultivariate(n, m, 0);
  MatrixXf A = MatrixXf::Constant(n, m);
  MatrixXf B = MatrixXf::Constant(n, p);
  MatrixXf Q = MatrixXf::Constant(p, m);
  MatrixXf R = MatrixXf::Constant(p, q);
  MatrixXf p0 = MatrixXf::Constant(n, n);
  VectorXf x0 = VectorXf::Zero(n);
  VectorXf x1 = VectorXf::Zero(m);

  void SetUp()
  {
    kalman.setInitial(x0, p0);
  }
  void TearDown() {}
};

TEST_F(KalmanFunctionality, returnsStateEstimate)
{
  ASSERT_EQ(kalman.getStateEstimate(), x1);
}

TEST_F(KalmanFunctionality, returnsStateCovariance)
{
  ASSERT_EQ(kalman.getStateCovariance(), p0);
}
