#pragma once

#include <Eigen/Dense>

namespace hyped {
namespace utils {
namespace math {

/**
 * @brief    This class is for filtering the data from sensors to smoothen it.
 */
class KalmanMultivariate {
 public:
  /**
   * @brief    Construct a new Kalman object with respective dimensions (with control)
   *
   * @param[in] n                       state dimensionality
   * @param[in] m                       measurement dimensionality
   * @param[in] k                       control dimensionality (default 0)
   */
  KalmanMultivariate(unsigned int n, unsigned int m, unsigned int k = 0);

  /**
   * @brief    Set dynamics model matrices (without control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] Q                       process noise covariance
   */
  void setDynamicsModel(Eigen::MatrixXf &A, Eigen::MatrixXf &Q);

  /**
   * @brief    Set dynamics model matrices (with control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] B                       control matrix
   * @param[in] Q                       process noise covariance
   */
  void setDynamicsModel(Eigen::MatrixXf &A, Eigen::MatrixXf &B, Eigen::MatrixXf &Q);

  /**
   * @brief    Set measurement model matrices
   *
   * @param[in] H                       measurement matrix
   * @param[in] R                       measurement noise covariance
   */
  void setMeasurementModel(Eigen::MatrixXf &H, Eigen::MatrixXf &R);

  /**
   * @brief    Set model matrices (without control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] Q                       process noise covariance
   * @param[in] H                       measurement matrix
   * @param[in] R                       measurement noise covariance
   */
  void setModels(Eigen::MatrixXf &A, Eigen::MatrixXf &Q, Eigen::MatrixXf &H, Eigen::MatrixXf &R);

  /**
   * @brief    Set model matrices (with control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] B                       control matrix
   * @param[in] Q                       process noise covariance
   * @param[in] H                       measurement matrix
   * @param[in] R                       measurement noise covariance
   */
  void setModels(Eigen::MatrixXf &A, Eigen::MatrixXf &B, Eigen::MatrixXf &Q, Eigen::MatrixXf &H,
                 Eigen::MatrixXf &R);

  /**
   * @brief    Update state transition matrix
   *
   * @param[in] A                       state transition matrix
   */
  void updateA(Eigen::MatrixXf &A);

  /**
   * @brief    Update measurement covariance matrix
   *
   * @param[in] R                       measurement covariance matrix
   */
  void updateR(Eigen::MatrixXf &R);

  /**
   * @brief    Set initial beliefs
   *
   * @param[in] x0                      initial state belief
   * @param[in] P0                      initial state covariance (uncertainty)
   */
  void setInitial(Eigen::VectorXf &x0, Eigen::MatrixXf &P0);

  /**
   * @brief    Filter measurement and update state belief with covariance (without control)
   *
   * @param[in] z                       measurement vector
   */
  void filter(Eigen::VectorXf &z);

  /**
   * @brief    Filter measurement and update state belief with covariance (with control)
   *
   * @param[in] u                       control vector
   * @param[in] z                       measurement vector
   */
  void filter(Eigen::VectorXf &u, Eigen::VectorXf &z);

  /**
   * @brief     Get the state estimate
   *
   * @return    Returns the current state estimate
   */
  Eigen::VectorXf &getStateEstimate();

  /**
   * @brief     Get the state uncertainty
   *
   * @return    Returns the current state covariance
   */
  Eigen::MatrixXf &getStateCovariance();

 private:
  /* problem dimensions */
  Eigen::Index n_;  // state dimension
  Eigen::Index m_;  // measurement dimension
  Eigen::Index k_;  // control dimension (0 if not set)

  /* dynamics model matrices */
  Eigen::MatrixXf A_;  // state transition matrix: n x n
  Eigen::MatrixXf B_;  // control matrix: n x k
  Eigen::MatrixXf Q_;  // process noise covariance: n x n

  /* measurement model matrices */
  Eigen::MatrixXf H_;  // measurement matrix: m x n
  Eigen::MatrixXf R_;  // measurement noise covariance: m x m

  /* state estimates */
  Eigen::VectorXf x_;  // state vector: n x 1
  Eigen::MatrixXf P_;  // state covariance: n x n
  Eigen::MatrixXf I_;  // identity matrix: n x n

  /**
   * @brief    Predict state belief with covariance based on dynamics (without control)
   */
  void predict();

  /**
   * @brief    Predict state belief with covariance based on dynamics (with control)
   *
   * @param[in] u                       control vector
   */
  void predict(Eigen::VectorXf &u);

  /**
   * @brief    Correct state belief with covariance based on measurement
   *
   * @param[in] z                       measurement vector
   */
  void correct(Eigen::VectorXf &z);
};
}  // namespace math
}  // namespace utils
}  // namespace hyped
