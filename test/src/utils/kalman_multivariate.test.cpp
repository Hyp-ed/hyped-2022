/*
 * Author: Donald Jennings
 * Organisation: HYPED
 * Date: 22/11/2020
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
  unsigned int n = 2;
  unsigned int m = 3;
  unsigned int k = 1;

  KalmanMultivariate kalman = KalmanMultivariate(n, m, 0);
  VectorXf x0 = VectorXf::Zero(n);
  VectorXf x1 = VectorXf::Random(n);
  VectorXf z = VectorXf::Random(n);
  MatrixXf P0 = MatrixXf::Zero(n, n);
  MatrixXf A = MatrixXf::Random(n, n);
  MatrixXf B = MatrixXf::Random(n, k);
  MatrixXf Q = MatrixXf::Random(n, n);
  MatrixXf H = MatrixXf::Random(m, n);
  MatrixXf R = MatrixXf::Random(m, m);
  MatrixXf P = MatrixXf::Random(n, n);

  void SetUp()
  {
    kalman.setModels(A, Q, H, R);
    kalman.setInitial(x0, P);
  }
  void TearDown() {}
};

TEST_F(KalmanFunctionality, handlesZeroStateEstimate)
{
  ASSERT_EQ(kalman.getStateEstimate(), x0);
}

TEST_F(KalmanFunctionality, handlesArbitraryStateEstimate)
{
  kalman.setInitial(x1, P);
  ASSERT_EQ(kalman.getStateEstimate(), x1);
}

TEST_F(KalmanFunctionality, handlesArbitraryStateCovariance)
{
  ASSERT_EQ(kalman.getStateCovariance(), P);
}

TEST_F(KalmanFunctionality, handlesZeroStateCovariance)
{
  kalman.setInitial(x0, P0);
  ASSERT_EQ(kalman.getStateCovariance(), P0);
}

struct KalmanMathematics : public::testing::Test {
  unsigned int n = 3;
  unsigned int m = 2;
  unsigned int k = 1;

  protected:
    KalmanMultivariate kalmanMathWithoutControl = KalmanMultivariate(n, m, 0);
    KalmanMultivariate kalmanMathWithControl = KalmanMultivariate(n, m, k);
    VectorXf x1_Data[100];
    VectorXf z_Data[100];
    VectorXf u_Data[100];
    MatrixXf A_Data[100];
    MatrixXf B_Data[100];
    MatrixXf Q_Data[100];
    MatrixXf H_Data[100];
    MatrixXf R_Data[100];
    MatrixXf P_Data[100];

    void SetUp()
    {
      for (int i = 0; i < 100; i++) {
        VectorXf x1 = VectorXf::Random(n);
        x1_Data[i] = x1;
        VectorXf z = VectorXf::Random(m);
        z_Data[i] = z;
        VectorXf u = VectorXf::Random(k);
        u_Data[i] = u;
        MatrixXf A = MatrixXf::Random(n, n);
        A_Data[i] = A;
        MatrixXf B = MatrixXf::Random(n, k);
        B_Data[i] = B;
        MatrixXf Q = MatrixXf::Random(n, n);
        Q_Data[i] = Q;
        MatrixXf H = MatrixXf::Random(m, n);
        H_Data[i] = H;
        MatrixXf R = MatrixXf::Random(m, m);
        R_Data[i] = R;
        MatrixXf P = MatrixXf::Random(n, n);
        P_Data[i] = P;
      }
    }

  void TearDown() {}
};

TEST_F(KalmanMathematics, handlesFilterWithoutControl)
{
  for (int i = 0; i < 100; i++) {
    MatrixXf A = A_Data[i];
    MatrixXf H = H_Data[i];
    MatrixXf R = R_Data[i];
    MatrixXf Q = Q_Data[i];
    VectorXf z = z_Data[i];
    kalmanMathWithoutControl.setModels(A, Q, H, R);
    kalmanMathWithoutControl.setInitial(x1_Data[i], P_Data[i]);
    VectorXf x = kalmanMathWithoutControl.getStateEstimate();
    MatrixXf p = kalmanMathWithoutControl.getStateCovariance();
    kalmanMathWithoutControl.filter(z);
    x = A * x;
    p = A * p * A.transpose() + Q;

    MatrixXf K = (p * H.transpose()) * (H * p * H.transpose() + R).inverse();
    x = x + K * (z - H * x);

    ASSERT_EQ(kalmanMathWithoutControl.getStateEstimate(), x);
  }
}

TEST_F(KalmanMathematics, handlesFilterWithControl)
{
  for (int i = 0; i < 100; i ++) {
    MatrixXf A = A_Data[i];
    MatrixXf B = B_Data[i];
    MatrixXf Q = Q_Data[i];
    MatrixXf H = H_Data[i];
    MatrixXf R = R_Data[i];
    VectorXf z = z_Data[i];
    VectorXf u = u_Data[i];
    kalmanMathWithControl.setModels(A, B, Q, H, R);
    kalmanMathWithControl.setInitial(x1_Data[i], P_Data[i]);

    VectorXf x = kalmanMathWithControl.getStateEstimate();
    MatrixXf p = kalmanMathWithControl.getStateCovariance();
    kalmanMathWithControl.filter(u, z);
    x = A * x + B * u;
    p = (A * p * A.transpose()) + Q;

    MatrixXf K = (p * H.transpose()) * (H * p * H.transpose() + R).inverse();
    x = x + K * (z - H * x);

    ASSERT_EQ(kalmanMathWithControl.getStateEstimate(), x);
  }
}
