#include "kalman_filter.hpp"

namespace hyped {
namespace navigation {

constexpr float KalmanFilter::kInitialErrorVariance;
constexpr float KalmanFilter::kStateTransitionVariance;
constexpr float KalmanFilter::kTrackMeasurementVariance;
constexpr float KalmanFilter::kElevatorMeasurementVariance;
constexpr float KalmanFilter::kStationaryMeasurementVariance;

KalmanFilter::KalmanFilter(uint32_t n /*=3*/, uint32_t m /*=1*/, uint32_t k /*=0*/)
    : n_(n),
      m_(m),
      k_(k),
      kalmanFilter_(utils::math::KalmanMultivariate(n, m, k))
{
}

void KalmanFilter::setup()
{
  // setup dynamics & measurement models for stationary test
  Eigen::MatrixXf A = createStateTransitionMatrix(0.0);
  Eigen::MatrixXf Q = createStateTransitionCovarianceMatrix();
  Eigen::MatrixXf H = createMeasurementMatrix();

  // check system navigation run for R setup
  utils::System &sys = utils::System::getSystem();
  Eigen::MatrixXf R  = Eigen::MatrixXf::Zero(m_, m_);
  ;
  if (sys.official_run || sys.outside_run)
    R = createTrackMeasurementCovarianceMatrix();
  else if (sys.elevator_run)
    R = createElevatorMeasurementCovarianceMatrix();
  else if (sys.stationary_run)
    R = createStationaryMeasurementCovarianceMatrix();

  kalmanFilter_.setModels(A, Q, H, R);

  // setup initial estimates
  Eigen::VectorXf x = Eigen::VectorXf::Zero(n_);
  Eigen::MatrixXf P = createInitialErrorCovarianceMatrix();
  kalmanFilter_.setInitial(x, P);
}

void KalmanFilter::updateStateTransitionMatrix(double dt)
{
  Eigen::MatrixXf A = createStateTransitionMatrix(dt);
  kalmanFilter_.updateA(A);
}

void KalmanFilter::updateMeasurementCovarianceMatrix(double var)
{
  Eigen::MatrixXf R = Eigen::MatrixXf::Constant(m_, m_, var);
  kalmanFilter_.updateR(R);
}

const data::nav_t KalmanFilter::filter(data::nav_t z)
{
  Eigen::VectorXf vz(m_);
  vz(0) = z;
  kalmanFilter_.filter(vz);

  data::nav_t estimate = getEstimate();
  return estimate;
}

const data::nav_t KalmanFilter::filter(data::nav_t u, data::nav_t z)
{
  Eigen::VectorXf vu(k_);
  vu(0) = u;

  Eigen::VectorXf vz(m_);
  vz(0) = z;

  kalmanFilter_.filter(vu, vz);

  data::nav_t estimate = getEstimate();
  return estimate;
}

const Eigen::MatrixXf KalmanFilter::createInitialErrorCovarianceMatrix() const
{
  Eigen::MatrixXf P = Eigen::MatrixXf::Constant(n_, n_, kInitialErrorVariance);
  return P;
}

const Eigen::MatrixXf KalmanFilter::createStateTransitionMatrix(double dt) const
{
  Eigen::MatrixXf A = Eigen::MatrixXf::Zero(n_, n_);
  double acc_ddt = 0.5 * dt * dt;
  //  number of values for each acc, vel, pos: usually 1 or 3
  uint32_t num_values = n_ / 3;

  for (uint32_t i = 0; i < num_values; ++i) {
    // compute acceleration rows
    A(i, i) = 1.;

    // compute velocity rows
    A(i + num_values, i)         = dt;
    A(i + num_values, i + num_values) = 1.;

    // compute position rows
    A(i + 2 * num_values, i)             = acc_ddt;
    A(i + 2 * num_values, i + num_values)     = dt;
    A(i + 2 * num_values, i + 2 * num_values) = 1.;
  }
  A(0, 0) = 1.0;

  return A;
}

const Eigen::MatrixXf KalmanFilter::createMeasurementMatrix() const
{
  Eigen::MatrixXf H = Eigen::MatrixXf::Zero(m_, n_);
  for (uint32_t i = 0; i < m_; ++i) {
    H(i, i) = 1.;
  }
  return H;
}

const Eigen::MatrixXf KalmanFilter::createStateTransitionCovarianceMatrix() const
{
  Eigen::MatrixXf Q = Eigen::MatrixXf::Constant(n_, n_, kStateTransitionVariance);
  return Q;
}

const Eigen::MatrixXf KalmanFilter::createTrackMeasurementCovarianceMatrix() const
{
  Eigen::MatrixXf R = Eigen::MatrixXf::Constant(m_, m_, kTrackMeasurementVariance);
  return R;
}
const Eigen::MatrixXf KalmanFilter::createElevatorMeasurementCovarianceMatrix() const
{
  Eigen::MatrixXf R = Eigen::MatrixXf::Constant(m_, m_, kElevatorMeasurementVariance);
  return R;
}

const Eigen::MatrixXf KalmanFilter::createStationaryMeasurementCovarianceMatrix() const
{
  Eigen::MatrixXf R = Eigen::MatrixXf::Constant(m_, m_, kStationaryMeasurementVariance);
  return R;
}

const data::nav_t KalmanFilter::getEstimate()
{
  Eigen::VectorXf x = kalmanFilter_.getStateEstimate();
  data::nav_t estimate = x(0);
  return estimate;
}

const data::nav_t KalmanFilter::getEstimateVariance()
{
  Eigen::MatrixXf P = kalmanFilter_.getStateCovariance();
  data::nav_t covariance = P(0, 0);
  return covariance;
}

}  // namespace navigation
}  // namespace hyped