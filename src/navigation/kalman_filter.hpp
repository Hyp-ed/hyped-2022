/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Header for Kalman filter (interface for filter and filter setup)
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

#ifndef NAVIGATION_KALMAN_FILTER_HPP_
#define NAVIGATION_KALMAN_FILTER_HPP_

#include <random>
#include <Eigen/Dense>

#include "data/data.hpp"
#include "utils/system.hpp"
#include "utils/math/kalman_multivariate.hpp"

using Eigen::MatrixXf;
using Eigen::VectorXf;

namespace hyped {
using data::NavigationType;
using data::NavigationVector;
using utils::System;
using utils::math::KalmanMultivariate;

namespace navigation {

class KalmanFilter
{
  public:
    KalmanFilter(unsigned int n = 3, unsigned int m = 1, unsigned int k = 0);
    void setup();
    void updateStateTransitionMatrix(double dt);
    void updateMeasurementCovarianceMatrix(double var);
    const NavigationType filter(NavigationType z);
    const NavigationType filter(NavigationType u, NavigationType z);
    // transfer estimate to NavigationVector
    const NavigationType getEstimate();
    // transfer estimate variances to NavigationVector
    const NavigationType getEstimateVariance();
  private:
    unsigned int    n_;
    unsigned int    m_;
    unsigned int    k_;
    KalmanMultivariate  kalmanFilter_;

    // covariance matrix variances
    static constexpr float kInitialErrorVar = 0.5;
    static constexpr float kStateTransitionVar = 0.02;
    static constexpr float kTrackMeasurementVar = 0.001;
    static constexpr float kElevatorMeasurementVar = 0.12;
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
}}  // namespace hyped navigation

#endif  // NAVIGATION_KALMAN_FILTER_HPP_
