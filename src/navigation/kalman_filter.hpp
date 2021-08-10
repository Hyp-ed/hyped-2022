#pragma once

#include <random>

#include <Eigen/Dense>
#include <data/data.hpp>
#include <utils/math/kalman_multivariate.hpp>
#include <utils/system.hpp>

using Eigen::MatrixXf;
using Eigen::VectorXf;

namespace hyped {
using data::nav_t;
using data::NavigationVector;
using utils::System;
using utils::math::KalmanMultivariate;

namespace navigation {

class KalmanFilter {
 public:
  KalmanFilter(unsigned int n = 3, unsigned int m = 1, unsigned int k = 0);
  void setup();
  void updateStateTransitionMatrix(double dt);
  void updateMeasurementCovarianceMatrix(double var);
  const nav_t filter(nav_t z);
  const nav_t filter(nav_t u, nav_t z);
  // transfer estimate to NavigationVector
  const nav_t getEstimate();
  // transfer estimate variances to NavigationVector
  const nav_t getEstimateVariance();

 private:
  unsigned int n_;
  unsigned int m_;
  unsigned int k_;
  KalmanMultivariate kalmanFilter_;

  // covariance matrix variances
  static constexpr float kInitialErrorVar          = 0.5;
  static constexpr float kStateTransitionVar       = 0.02;
  static constexpr float kTrackMeasurementVar      = 0.001;
  static constexpr float kElevatorMeasurementVar   = 0.12;
  static constexpr float kStationaryMeasurementVar = 0.04;

  // create initial error covariance matrix P
  const MatrixXf createInitialErrorCovarianceMatrix();

  // create state transition matrix A
  const MatrixXf createStateTransitionMatrix(double dt);

  // create measurement matrix H
  const MatrixXf createMeasurementMatrix();

  // create state transition coveriance matrix Q
  const MatrixXf createStateTransitionCovarianceMatrix();

  // create measurement covariance matrices R
  const MatrixXf createTrackMeasurementCovarianceMatrix();
  const MatrixXf createElevatorMeasurementCovarianceMatrix();
  const MatrixXf createStationaryMeasurementCovarianceMatrix();
};
}  // namespace navigation
}  // namespace hyped
