#include <math.h>

#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include <utils/math/kalman_multivariate.hpp>
#include <utils/system.hpp>

using hyped::utils::math::KalmanMultivariate;

// -------------------------------------------------------------------------------------------------
// Functionality
// -------------------------------------------------------------------------------------------------

/**
 * Class used for test fixtures testing the functionality of Kalman_Multivaraite.hpp. Constructor
 * sets the values of relevant Vector and Matrix objects using Random values. Naming for these
 * vectors and matrices are based off the implementation in kalman_filter.cpp and
 * the variable names within the kalman_multivariate.hpp
 */
class KalmanFunctionality : public ::testing::Test {
 protected:
  unsigned int n = 2;
  unsigned int m = 3;
  unsigned int k = 1;

  KalmanMultivariate kalman           = KalmanMultivariate(n, m, 0);
  Eigen::VectorXf x0                  = Eigen::VectorXf::Zero(n);
  Eigen::VectorXf x1                  = Eigen::VectorXf::Random(n);
  Eigen::VectorXf z                   = Eigen::VectorXf::Random(n);
  Eigen::MatrixXf P0                  = Eigen::MatrixXf::Zero(n, n);
  Eigen::MatrixXf A                   = Eigen::MatrixXf::Random(n, n);
  Eigen::MatrixXf B                   = Eigen::MatrixXf::Random(n, k);
  Eigen::MatrixXf Q                   = Eigen::MatrixXf::Random(n, n);
  Eigen::MatrixXf H                   = Eigen::MatrixXf::Random(m, n);
  Eigen::MatrixXf R                   = Eigen::MatrixXf::Random(m, m);
  Eigen::MatrixXf P                   = Eigen::MatrixXf::Random(n, n);
  std::string zero_state_estimate_err = "Should handle zero vector as state estimate";
  std::string arb_state_estimate_err  = "Should handle any arbitrary vector as state estimate";
  std::string zero_covariance_err     = "Should handle zero state covariance";
  std::string arb_covariance_err      = "Should handle any arbitrary state covariance";
  std::string update_err              = "Should allow updating the matrix";

  void SetUp()
  {
    // Assigns the matrices and vectors to the properties within the kalman object.
    kalman.setModels(A, Q, H, R);
    kalman.setInitial(x0, P);
  }
  void TearDown() {}
};

/**
 * Test fixture used for determining whether Kalman handles the zero vector as input for the state
 * estimates. Should accept
 */
TEST_F(KalmanFunctionality, handlesZeroStateEstimate)
{
  ASSERT_EQ(kalman.getStateEstimate(), x0) << zero_state_estimate_err;
}

/**
 * Test fixture used for determining whether Kalman handles an arbitrary vector (using Random()) as
 * input for the state estimates. This should always be accepted.
 */
TEST_F(KalmanFunctionality, handlesArbitraryStateEstimate)
{
  kalman.setInitial(x1, P);
  ASSERT_EQ(kalman.getStateEstimate(), x1) << arb_state_estimate_err;
}

/**
 * Test fixture used for determining whether Kalman handles the zero matrix as input for the state
 * covariance. This should always be accepted.
 */
TEST_F(KalmanFunctionality, handlesZeroStateCovariance)
{
  kalman.setInitial(x0, P0);
  ASSERT_EQ(kalman.getStateCovariance(), P0) << zero_covariance_err;
}

/**
 * Test fixture used for determining whether Kalman handles an arbitrary matrix (using Random()) as
 * input for the state covariance. This should always be accepted.
 */
TEST_F(KalmanFunctionality, handlesArbitraryStateCovariance)
{
  ASSERT_EQ(kalman.getStateCovariance(), P) << arb_covariance_err;
}

/**
 * Test fixture used for determining whether Kalman handles an update on the State transition matrix
 * It will be updated twice, first to the Zero Matrix and then to to Identity Matrix
 */
TEST_F(KalmanFunctionality, handlesUpdateInA)
{
  KalmanMultivariate kalman_two = KalmanMultivariate(n, m, 0);
  Eigen::VectorXf x1            = Eigen::VectorXf::Random(n);
  while (x1 == Eigen::VectorXf::Zero(n)) {
    x1 = Eigen::VectorXf::Random(n, n);
  }
  A = Eigen::MatrixXf::Random(n, n);
  while (A == Eigen::MatrixXf::Zero(n, n) || A == Eigen::MatrixXf::Identity(n, n)) {
    A = Eigen::MatrixXf::Random(n, n);
  }
  B = Eigen::MatrixXf::Zero(n, k);
  Q = Eigen::MatrixXf::Zero(n, n);
  H = Eigen::MatrixXf::Zero(m, n);
  R = Eigen::MatrixXf::Identity(m, m);
  kalman.setModels(A, Q, H, R);
  kalman_two.setModels(A, Q, H, R);
  A = Eigen::MatrixXf::Zero(n, n);
  kalman_two.updateA(A);
  kalman.setInitial(x1, P);
  kalman_two.setInitial(x1, P);
  z = Eigen::VectorXf::Zero(m);
  kalman_two.filter(z);
  kalman.filter(z);
  ASSERT_EQ(kalman_two.getStateEstimate(), Eigen::VectorXf::Zero(n)) << update_err;
  ASSERT_NE(kalman_two.getStateEstimate(), kalman.getStateEstimate()) << update_err;
  A = Eigen::MatrixXf::Identity(n, n);
  kalman_two.updateA(A);
  kalman_two.setInitial(x1, P);
  kalman_two.filter(z);
  ASSERT_EQ(kalman_two.getStateEstimate(), x1) << update_err;
  ASSERT_NE(kalman_two.getStateEstimate(), kalman.getStateEstimate()) << update_err;
}

/**
 * Test fixture used for determining whether Kalman handles an update on the Noise covariance matrix
 * It will be updated once to a random matrix and it starts being the identity.
 */
TEST_F(KalmanFunctionality, handlesUpdateInR)
{
  KalmanMultivariate kalman_two = KalmanMultivariate(n, m, 0);
  Eigen::VectorXf x1            = Eigen::VectorXf::Random(n);
  while (x1 == Eigen::VectorXf::Zero(n)) {
    x1 = Eigen::VectorXf::Random(n, n);
  }
  A = Eigen::MatrixXf::Identity(n, n);
  B = Eigen::MatrixXf::Zero(n, k);
  Q = Eigen::MatrixXf::Zero(n, n);
  H = Eigen::MatrixXf::Random(m, n);
  R = Eigen::MatrixXf::Identity(m, m);
  kalman.setModels(A, Q, H, R);
  kalman_two.setModels(A, Q, H, R);
  R = Eigen::MatrixXf::Random(m, m);
  while (R == Eigen::MatrixXf::Zero(m, m) || R == Eigen::MatrixXf::Identity(m, m)
         || R.determinant() == 0) {
    R = Eigen::MatrixXf::Random(m, m);
  }
  kalman_two.updateR(R);
  kalman.setInitial(x1, P);
  kalman_two.setInitial(x1, P);
  z = Eigen::VectorXf::Random(m);
  kalman_two.filter(z);
  kalman.filter(z);
  ASSERT_NE(kalman_two.getStateEstimate(), kalman.getStateEstimate()) << update_err;
}
// -------------------------------------------------------------------------------------------------
// Mathematical Properties
// -------------------------------------------------------------------------------------------------

/**
 * Class used for testing the mathematical properties of kalman_multivariate. Constructor initiates
 * an array of Vectors and Matrices to store arbitrary vectors for testing purposes. The arrays are
 * then populated with values so they can be tested.
 */
class KalmanMathematics : public ::testing::Test {
 protected:
  unsigned int n = 3;
  unsigned int m = 2;
  unsigned int k = 1;

  KalmanMultivariate kalmanMathWithoutControl = KalmanMultivariate(n, m, 0);
  KalmanMultivariate kalmanMathWithControl    = KalmanMultivariate(n, m, k);
  static constexpr int NUM_TESTDATA           = 50;
  Eigen::VectorXf x1_Data[NUM_TESTDATA];
  Eigen::VectorXf z_Data[NUM_TESTDATA];
  Eigen::VectorXf u_Data[NUM_TESTDATA];
  Eigen::MatrixXf A_Data[NUM_TESTDATA];
  Eigen::MatrixXf B_Data[NUM_TESTDATA];
  Eigen::MatrixXf Q_Data[NUM_TESTDATA];
  Eigen::MatrixXf H_Data[NUM_TESTDATA];
  Eigen::MatrixXf R_Data[NUM_TESTDATA];
  Eigen::MatrixXf P_Data[NUM_TESTDATA];
  Eigen::MatrixXf I                       = Eigen::MatrixXf::Identity(n, n);
  std::string expected_state_estimate_err = "State estimate isnt same as expected state estimate";
  std::string expected_covariance_err     = "Covariance isnt the same as expected state covariance";
  void SetUp()
  {
    // Populates the arrays defined above with 50 random values
    for (size_t i = 0; i < 50; ++i) {
      x1_Data[i] = Eigen::VectorXf::Random(n);
      z_Data[i]  = Eigen::VectorXf::Random(m);
      u_Data[i]  = Eigen::VectorXf::Random(k);
      A_Data[i]  = Eigen::MatrixXf::Random(n, n);
      B_Data[i]  = Eigen::MatrixXf::Random(n, k);
      Q_Data[i]  = Eigen::MatrixXf::Random(n, n);
      H_Data[i]  = Eigen::MatrixXf::Random(m, n);
      R_Data[i]  = Eigen::MatrixXf::Random(m, m);
      P_Data[i]  = Eigen::MatrixXf::Random(n, n);
    }
  }

  void TearDown() {}
};
/**
 * Test fixture for testing whether the filter (with control) updates the X Vector and P Matrix
 * accordingly. Maths has been replicated throughout since to track the values updating and final
 * assert at the end checks that the values are as expected.
 * Checks that both the X vector and P Matrix are updated appropriately
 */
TEST_F(KalmanMathematics, handlesSeveralFiltersWithControl)
{
  Eigen::MatrixXf A = A_Data[0];
  Eigen::MatrixXf B = B_Data[0];
  Eigen::MatrixXf Q = Q_Data[0];
  Eigen::MatrixXf H = H_Data[0];
  Eigen::MatrixXf R = R_Data[0];
  Eigen::VectorXf z = z_Data[0];
  Eigen::VectorXf u = u_Data[0];
  kalmanMathWithControl.setModels(A, B, Q, H, R);
  kalmanMathWithControl.setInitial(x1_Data[0], P_Data[0]);
  Eigen::VectorXf x = kalmanMathWithControl.getStateEstimate();
  Eigen::MatrixXf p = kalmanMathWithControl.getStateCovariance();
  for (size_t i = 0; i < 50; ++i) {
    kalmanMathWithControl.filter(u, z);
    // Mimicks filter(Eigen::VectorXf& u, Eigen::VectorXf& z)
    x = A * x + B * u;
    p = (A * p * A.transpose()) + Q;
    // Mimicks correct()
    Eigen::MatrixXf K = (p * H.transpose()) * (H * p * H.transpose() + R).inverse();
    x                 = x + K * (z - H * x);
    p                 = (I - K * H) * p;
    ASSERT_EQ(kalmanMathWithControl.getStateEstimate(), x) << expected_state_estimate_err;
    ASSERT_EQ(kalmanMathWithControl.getStateCovariance(), p) << expected_covariance_err;
  }
}
/**
 * Test fixture for testing whether the filter (without control) updates the X Vector and P Matrix
 * accordingly. Maths has been replicated throughout since to track the values updating and final
 * assert at the end checks that the values are as expected.
 * Checks that both the X vector and P Matrix are updated appropriately
 */
TEST_F(KalmanMathematics, handlesSeveralFiltersWithoutControl)
{
  Eigen::MatrixXf A = A_Data[0];
  Eigen::MatrixXf B = B_Data[0];
  Eigen::MatrixXf Q = Q_Data[0];
  Eigen::MatrixXf H = H_Data[0];
  Eigen::MatrixXf R = R_Data[0];
  Eigen::VectorXf z = z_Data[0];
  Eigen::VectorXf u = u_Data[0];
  kalmanMathWithoutControl.setModels(A, Q, H, R);
  kalmanMathWithoutControl.setInitial(x1_Data[0], P_Data[0]);
  Eigen::VectorXf x = kalmanMathWithoutControl.getStateEstimate();
  Eigen::MatrixXf p = kalmanMathWithoutControl.getStateCovariance();
  for (size_t i = 0; i < NUM_TESTDATA; ++i) {
    kalmanMathWithoutControl.filter(z);
    // Mimicks filter(Eigen::VectorXf& u, Eigen::VectorXf& z)
    x = A * x;
    p = (A * p * A.transpose()) + Q;
    // Mimicks correct()
    Eigen::MatrixXf K = (p * H.transpose()) * (H * p * H.transpose() + R).inverse();
    x                 = x + K * (z - H * x);
    p                 = (I - K * H) * p;
    ASSERT_EQ(kalmanMathWithoutControl.getStateEstimate(), x) << expected_state_estimate_err;
    ASSERT_EQ(kalmanMathWithoutControl.getStateCovariance(), p) << expected_covariance_err;
  }
}

/**
 * Test fixture for testing whether the filter (without control) updates the X Vector and P Matrix
 * accordingly. Maths has been replicated throughout since to track the values updating and final
 * assert at the end checks that the values are as expected.
 * Checks that both the X vector and P Matrix are updated appropriately
 */
TEST_F(KalmanMathematics, handlesFilterWithoutControl)
{
  for (size_t i = 0; i < NUM_TESTDATA; ++i) {
    Eigen::MatrixXf A = A_Data[i];
    Eigen::MatrixXf H = H_Data[i];
    Eigen::MatrixXf R = R_Data[i];
    Eigen::MatrixXf Q = Q_Data[i];
    Eigen::VectorXf z = z_Data[i];
    kalmanMathWithoutControl.setModels(A, Q, H, R);
    kalmanMathWithoutControl.setInitial(x1_Data[i], P_Data[i]);
    Eigen::VectorXf x = kalmanMathWithoutControl.getStateEstimate();
    Eigen::MatrixXf p = kalmanMathWithoutControl.getStateCovariance();
    kalmanMathWithoutControl.filter(z);

    // Mimicks predict(Eigen::VectorXf&)
    x = A * x;
    p = A * p * A.transpose() + Q;

    // Mimicks correct()
    Eigen::MatrixXf K = (p * H.transpose()) * (H * p * H.transpose() + R).inverse();
    x                 = x + K * (z - H * x);
    p                 = (I - K * H) * p;

    ASSERT_EQ(kalmanMathWithoutControl.getStateEstimate(), x) << expected_state_estimate_err;
    ASSERT_EQ(kalmanMathWithoutControl.getStateCovariance(), p) << expected_covariance_err;
  }
}

/**
 * Test fixture for testing whether the filter (with control) updates the X Vector and P Matrix
 * accordingly. Maths has been replicated throughout since to track the values updating and final
 * assert at the end checks that the values are as expected.
 * Checks that both the X vector and P Matrix are updated appropriately
 */
TEST_F(KalmanMathematics, handlesFilterWithControl)
{
  for (size_t i = 0; i < NUM_TESTDATA; ++i) {
    Eigen::MatrixXf A = A_Data[i];
    Eigen::MatrixXf B = B_Data[i];
    Eigen::MatrixXf Q = Q_Data[i];
    Eigen::MatrixXf H = H_Data[i];
    Eigen::MatrixXf R = R_Data[i];
    Eigen::VectorXf z = z_Data[i];
    Eigen::VectorXf u = u_Data[i];
    kalmanMathWithControl.setModels(A, B, Q, H, R);
    kalmanMathWithControl.setInitial(x1_Data[i], P_Data[i]);
    Eigen::VectorXf x = kalmanMathWithControl.getStateEstimate();
    Eigen::MatrixXf p = kalmanMathWithControl.getStateCovariance();
    kalmanMathWithControl.filter(u, z);

    // Mimicks filter(Eigen::VectorXf& u, Eigen::VectorXf& z)
    x = A * x + B * u;
    p = (A * p * A.transpose()) + Q;

    // Mimicks correct()
    Eigen::MatrixXf K = (p * H.transpose()) * (H * p * H.transpose() + R).inverse();
    x                 = x + K * (z - H * x);
    p                 = (I - K * H) * p;

    ASSERT_EQ(kalmanMathWithControl.getStateEstimate(), x) << expected_state_estimate_err;
    ASSERT_EQ(kalmanMathWithControl.getStateCovariance(), p) << expected_covariance_err;
  }
}
/**
 * Class used for testing the properties of the kalman filter under identity operations, those that
 * do not modify neither the state vector nor the vocariance matrix on each filter().
 */
class KalmanIdentity : public ::testing::Test {
 protected:
  unsigned int n = 2;
  unsigned int m = 3;
  unsigned int k = 1;

  KalmanMultivariate kalman         = KalmanMultivariate(n, m, k);
  static constexpr int NUM_TESTDATA = 50;
  Eigen::VectorXf x0                = Eigen::VectorXf::Random(n);
  Eigen::VectorXf x1                = Eigen::VectorXf::Random(n);
  Eigen::VectorXf z                 = Eigen::VectorXf::Zero(m);
  Eigen::VectorXf u                 = Eigen::VectorXf::Random(k);

  Eigen::MatrixXf P0 = Eigen::MatrixXf::Random(n, n);
  Eigen::MatrixXf A  = Eigen::MatrixXf::Identity(n, n);
  Eigen::MatrixXf B  = Eigen::MatrixXf::Zero(n, k);
  Eigen::MatrixXf Q  = Eigen::MatrixXf::Zero(n, n);
  Eigen::MatrixXf H  = Eigen::MatrixXf::Zero(m, n);
  Eigen::MatrixXf R  = Eigen::MatrixXf::Identity(m, m);
  Eigen::MatrixXf P  = Eigen::MatrixXf::Random(n, n);
  std::string identity_err
    = "The filter() should not have any effect on the covariance and state given these conditions";

  void SetUp()
  {
    // Assigns the matrices and vectors to the properties within the kalman object.
    if (P.determinant() == 0) { P = Eigen::MatrixXf::Random(n, n); }
    kalman.setModels(A, B, Q, H, R);
    kalman.setInitial(x0, P);
  }
  void TearDown() {}
};

/**
 * Test fixture for testing whether the filter updates the X Vector and P Matrix
 * accordingly, in this case the matrices have been created in a way that the state
 * vector and the covariance should be mantained constant through all the filter() runs.
 * Maths has been replicated throughout since to track the values updating and final
 * assert at the end checks that the values are as expected.
 * Checks that both the X vector and P Matrix are updated appropriately
 */
TEST_F(KalmanIdentity, handlesIdentity)
{
  for (size_t i = 0; i < NUM_TESTDATA; ++i) {
    kalman.filter(u, z);
    ASSERT_EQ(kalman.getStateEstimate(), x0) << identity_err;
    ASSERT_EQ(kalman.getStateCovariance(), P) << identity_err;
  }
}

/**
 * Class used for testing the excpetions thrown by the class KalmanMultivariate
 */
class KalmanExceptions : public ::testing::Test {
 public:
  unsigned int n = 2;
  unsigned int m = 3;
  unsigned int k = 1;
  unsigned int c = 10;

  KalmanMultivariate kalman = KalmanMultivariate(n, m, 0);
  Eigen::VectorXf x0        = Eigen::VectorXf::Zero(c);
  Eigen::VectorXf x1        = Eigen::VectorXf::Random(n);
  Eigen::VectorXf z         = Eigen::VectorXf::Random(n);
  Eigen::MatrixXf P0        = Eigen::MatrixXf::Zero(n, n);
  Eigen::MatrixXf A         = Eigen::MatrixXf::Random(n, n);
  Eigen::MatrixXf B         = Eigen::MatrixXf::Random(n, k);
  Eigen::MatrixXf Q         = Eigen::MatrixXf::Random(n, n);
  Eigen::MatrixXf H         = Eigen::MatrixXf::Random(m, n);
  Eigen::MatrixXf R         = Eigen::MatrixXf::Random(m, m);
  Eigen::MatrixXf P         = Eigen::MatrixXf::Random(n, n);
  std::string exception_err
    = "The matrices used have the wrong dimensions, an invalid_argument excpetion is expected";

  void SetUp() {}
  void TearDown() {}
};

/**
 * Test fixture for testing whether Kalman can andle the case of matrices having wrong dimensions,
 * This test will change the dimensions of the different matrices and it will chek that the
 * programme throws and appropriate exception.
 */
TEST_F(KalmanExceptions, handlesDimensionalityIssues)
{
  EXPECT_THROW(kalman.setInitial(x0, P0), std::invalid_argument) << exception_err;
  P0                 = Eigen::MatrixXf::Zero(k, m);
  Eigen::VectorXf x0 = Eigen::VectorXf::Zero(n);
  EXPECT_THROW(kalman.setInitial(x0, P0), std::invalid_argument) << exception_err;
  Eigen::MatrixXf A = Eigen::MatrixXf::Random(k, m);
  EXPECT_THROW(kalman.updateA(A), std::invalid_argument) << exception_err;
  EXPECT_THROW(kalman.setModels(A, Q, H, R), std::invalid_argument) << exception_err;
  A = Eigen::MatrixXf::Random(n, n);
  Q = Eigen::MatrixXf::Random(k, m);
  EXPECT_THROW(kalman.setModels(A, Q, H, R), std::invalid_argument) << exception_err;
  Q = Eigen::MatrixXf::Random(n, n);
  H = Eigen::MatrixXf::Random(k, m);
  EXPECT_THROW(kalman.setModels(A, Q, H, R), std::invalid_argument) << exception_err;
  H = Eigen::MatrixXf::Random(n, n);
  R = Eigen::MatrixXf::Random(k, m);
  EXPECT_THROW(kalman.setModels(A, Q, H, R), std::invalid_argument) << exception_err;
  A = Eigen::MatrixXf::Random(k, m);
  EXPECT_THROW(kalman.setModels(A, B, Q, H, R), std::invalid_argument) << exception_err;
  A = Eigen::MatrixXf::Random(n, n);
  Q = Eigen::MatrixXf::Random(k, m);
  EXPECT_THROW(kalman.setModels(A, B, Q, H, R), std::invalid_argument) << exception_err;
  Q = Eigen::MatrixXf::Random(n, n);
  H = Eigen::MatrixXf::Random(k, m);
  EXPECT_THROW(kalman.setModels(A, B, Q, H, R), std::invalid_argument) << exception_err;
  H = Eigen::MatrixXf::Random(n, n);
  R = Eigen::MatrixXf::Random(k, m);
  EXPECT_THROW(kalman.updateR(R), std::invalid_argument);
  EXPECT_THROW(kalman.setModels(A, B, Q, H, R), std::invalid_argument) << exception_err;
  R = Eigen::MatrixXf::Random(m, m);
  B = Eigen::MatrixXf::Random(k, m);
  EXPECT_THROW(kalman.setModels(A, B, Q, H, R), std::invalid_argument) << exception_err;
}
