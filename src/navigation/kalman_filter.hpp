#pragma once

#include <random>

#include <Eigen/Dense>
#include <data/data.hpp>
#include <utils/math/kalman_multivariate.hpp>
#include <utils/system.hpp>


namespace hyped {
namespace navigation {

class KalmanFilter {
 public:
  KalmanFilter(uint32_t n = 3, uint32_t m = 1, uint32_t k = 0);
  void setup();
  void updateStateTransitionMatrix(double dt);
  void updateMeasurementCovarianceMatrix(double var);
  const data::nav_t filter(data::nav_t z);
  const data::nav_t filter(data::nav_t u, data::nav_t z);
  // transfer estimate to NavigationVector
  const data::nav_t getEstimate();
  // transfer estimate variances to NavigationVector
  const data::nav_t getEstimateVariance();

 private:
  // state dimensionality
  uint32_t n_;
  // measurement dimensionality
  uint32_t m_;
  // control dimensionality default = 0
  uint32_t k_;
  utils::math::KalmanMultivariate kalmanFilter_;

  // covariance matrix variances
  static constexpr float kInitialErrorVariance          = 0.5;
  static constexpr float kStateTransitionVariance       = 0.02;
  static constexpr float kTrackMeasurementVariance      = 0.001;
  static constexpr float kElevatorMeasurementVariance   = 0.12;
  static constexpr float kStationaryMeasurementVariance = 0.04;

  // create initial error covariance matrix P
  const Eigen::MatrixXf createInitialErrorCovarianceMatrix() const;

  // create state transition matrix A
  const Eigen::MatrixXf createStateTransitionMatrix(double dt) const;

  // create measurement matrix H
  const Eigen::MatrixXf createMeasurementMatrix() const;

  // create state transition coveriance matrix Q
  const Eigen::MatrixXf createStateTransitionCovarianceMatrix() const;

  // create measurement covariance matrices R
  const Eigen::MatrixXf createTrackMeasurementCovarianceMatrix() const;
  const Eigen::MatrixXf createElevatorMeasurementCovarianceMatrix() const;
  const Eigen::MatrixXf createStationaryMeasurementCovarianceMatrix() const;
};
}  // namespace navigation
}  // namespace hyped