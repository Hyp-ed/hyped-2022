#include "kalman_multivariate.hpp"

namespace hyped {
namespace utils {
namespace math {

KalmanMultivariate::KalmanMultivariate(unsigned int n, unsigned int m, unsigned int k)
    : n_(n),
      m_(m),
      k_(k)
{
}

void KalmanMultivariate::setDynamicsModel(MatrixXf &A, MatrixXf &Q)
{
  if (A.cols() != n_ || A.rows() != n_ || Q.cols() != n_ || Q.rows() != n_) {
    throw std::invalid_argument("Wrong dimension of the Matrices");
  } else {
    A_ = A;
    Q_ = Q;
  }
}

void KalmanMultivariate::setDynamicsModel(MatrixXf &A, MatrixXf &B, MatrixXf &Q)
{
  if (A.cols() != n_ || A.rows() != n_ || Q.cols() != n_ || Q.rows() != n_ || B.rows() != n_
      || B.cols() != k_) {
    throw std::invalid_argument("Wrong dimension of the Matrices");
  } else {
    A_ = A;
    B_ = B;
    Q_ = Q;
  }
}

void KalmanMultivariate::setMeasurementModel(MatrixXf &H, MatrixXf &R)
{
  if (R.cols() != m_ || R.rows() != m_ || H.rows() != m_ || H.cols() != n_) {
    throw std::invalid_argument("Wrong dimension of the Matrices");
  } else {
    H_ = H;
    R_ = R;
  }
}
void KalmanMultivariate::setModels(MatrixXf &A, MatrixXf &Q, MatrixXf &H, MatrixXf &R)
{
  setDynamicsModel(A, Q);
  setMeasurementModel(H, R);
}

void KalmanMultivariate::setModels(MatrixXf &A, MatrixXf &B, MatrixXf &Q, MatrixXf &H, MatrixXf &R)
{
  setDynamicsModel(A, B, Q);
  setMeasurementModel(H, R);
}

void KalmanMultivariate::updateA(MatrixXf &A)
{
  if (A.cols() != n_ || A.rows() != n_) {
    throw std::invalid_argument("Wrong dimension of the Matrices");
  } else {
    A_ = A;
  }
}

void KalmanMultivariate::updateR(MatrixXf &R)
{
  if (R.cols() != m_ || R.rows() != m_) {
    throw std::invalid_argument("Wrong dimension of the Matrices");
  } else {
    R_ = R;
  }
}

void KalmanMultivariate::setInitial(VectorXf &x0, MatrixXf &P0)
{
  if (x0.rows() != n_ || P0.rows() != n_ || P0.cols() != n_) {
    throw std::invalid_argument("Dimension of Matrices not correct");
  } else {
    x_ = x0;
    P_ = P0;
    I_ = MatrixXf::Identity(n_, n_);
  }
}

void KalmanMultivariate::predict()
{
  x_ = A_ * x_;
  P_ = A_ * P_ * A_.transpose() + Q_;
}

void KalmanMultivariate::predict(VectorXf &u)
{
  x_ = A_ * x_ + B_ * u;
  P_ = (A_ * P_ * A_.transpose()) + Q_;
}

void KalmanMultivariate::correct(VectorXf &z)
{
  MatrixXf K = (P_ * H_.transpose()) * (H_ * P_ * H_.transpose() + R_).inverse();
  x_         = x_ + K * (z - H_ * x_);
  P_         = (I_ - K * H_) * P_;
}

void KalmanMultivariate::filter(VectorXf &z)
{
  predict();
  correct(z);
}

void KalmanMultivariate::filter(VectorXf &u, VectorXf &z)
{
  predict(u);
  correct(z);
}

VectorXf &KalmanMultivariate::getStateEstimate()
{
  return x_;
}

MatrixXf &KalmanMultivariate::getStateCovariance()
{
  return P_;
}

}  // namespace math
}  // namespace utils
}  // namespace hyped
