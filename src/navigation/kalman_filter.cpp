/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Kalman filter (interface for filter and filter setup)
 *
 *  Copyright 2019 HYPED
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *  except in compliance with the License. You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under
 *  the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *  either express or implied. See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "kalman_filter.hpp"

namespace hyped {
namespace navigation {

constexpr float KalmanFilter::kInitialErrorVar;
constexpr float KalmanFilter::kStateTransitionVar;
constexpr float KalmanFilter::kTrackMeasurementVar;
constexpr float KalmanFilter::kElevatorMeasurementVar;
constexpr float KalmanFilter::kStationaryMeasurementVar;

KalmanFilter::KalmanFilter(unsigned int n/*=3*/, unsigned int m/*=1*/, unsigned int k/*=0*/)
  : n_(n),
    m_(m),
    k_(k),
    kalmanFilter_(KalmanMultivariate(n, m, k))
{}

void KalmanFilter::setup()
{
  // setup dynamics & measurement models for stationary test
  MatrixXf A = createStateTransitionMatrix(0.0);
  MatrixXf Q = createStateTransitionCovarianceMatrix();
  MatrixXf H = createMeasurementMatrix();

  // check system navigation run for R setup
  System &sys = System::getSystem();
  MatrixXf R = MatrixXf::Zero(m_, m_);;
  if (sys.official_run || sys.outside_run) R = createTrackMeasurementCovarianceMatrix();
  else if (sys.elevator_run) R = createElevatorMeasurementCovarianceMatrix();
  else if (sys.stationary_run) R = createStationaryMeasurementCovarianceMatrix();

  kalmanFilter_.setModels(A, Q, H, R);

  // setup initial estimates
  VectorXf x = VectorXf::Zero(n_);
  MatrixXf P = createInitialErrorCovarianceMatrix();
  kalmanFilter_.setInitial(x, P);
}

void KalmanFilter::updateStateTransitionMatrix(double dt)
{
  MatrixXf A = createStateTransitionMatrix(dt);
  kalmanFilter_.updateA(A);
}

void KalmanFilter::updateMeasurementCovarianceMatrix(double var)
{
  MatrixXf R = MatrixXf::Constant(m_, m_, var);
  kalmanFilter_.updateR(R);
}

const NavigationType KalmanFilter::filter(NavigationType z)
{
  VectorXf vz(m_);
  vz(0) = z;
  kalmanFilter_.filter(vz);

  NavigationType estimate = getEstimate();
  return estimate;
}

const NavigationType KalmanFilter::filter(NavigationType u, NavigationType z)
{
  VectorXf vu(k_);
  vu(0) = u;

  VectorXf vz(m_);
  vz(0) = z;

  kalmanFilter_.filter(vu, vz);

  NavigationType estimate = getEstimate();
  return estimate;
}

const MatrixXf KalmanFilter::createInitialErrorCovarianceMatrix()
{
  MatrixXf P = MatrixXf::Constant(n_, n_, kInitialErrorVar);
  return P;
}

const MatrixXf KalmanFilter::createStateTransitionMatrix(double dt)
{
  MatrixXf A = MatrixXf::Zero(n_, n_);
  double acc_ddt = 0.5 * dt * dt;
  //  number of values for each acc, vel, pos: usually 1 or 3
  unsigned int n_val = n_ / 3;

  for (unsigned int i = 0; i < n_val; i++) {
      // compute acc rows
      A(i, i) = 1.;

      // compute vel rows
      A(i + n_val, i) = dt;
      A(i + n_val, i + n_val) = 1.;

      // compute pos rows
      A(i + 2 * n_val, i) = acc_ddt;
      A(i + 2 * n_val, i + n_val) = dt;
      A(i + 2 * n_val, i + 2 * n_val) = 1.;
  }
  A(0, 0) = 1.0;

  return A;
}

const MatrixXf KalmanFilter::createMeasurementMatrix()
{
  MatrixXf H = MatrixXf::Zero(m_, n_);
  for (unsigned int i = 0; i < m_; i++) {
    H(i, i) = 1.;
  }
  return H;
}

const MatrixXf KalmanFilter::createStateTransitionCovarianceMatrix()
{
  MatrixXf Q = MatrixXf::Constant(n_, n_, kStateTransitionVar);
  return Q;
}

const MatrixXf KalmanFilter::createTrackMeasurementCovarianceMatrix()
{
    MatrixXf R = MatrixXf::Constant(m_, m_, kTrackMeasurementVar);
    return R;
}
const MatrixXf KalmanFilter::createElevatorMeasurementCovarianceMatrix()
{
  MatrixXf R = MatrixXf::Constant(m_, m_, kElevatorMeasurementVar);
  return R;
}

const MatrixXf KalmanFilter::createStationaryMeasurementCovarianceMatrix()
{
  MatrixXf R = MatrixXf::Constant(m_, m_, kStationaryMeasurementVar);
  return R;
}

const NavigationType KalmanFilter::getEstimate()
{
  VectorXf x = kalmanFilter_.getStateEstimate();
  NavigationType est = x(0);
  return est;
}

const NavigationType KalmanFilter::getEstimateVariance()
{
  MatrixXf P = kalmanFilter_.getStateCovariance();
  NavigationType var = P(0, 0);
  return var;
}

}}  // namespace hyped navigation
