#include "navigation.hpp"

#include <algorithm>

#include <utils/concurrent/thread.hpp>
#include <utils/timer.hpp>

namespace hyped {

namespace navigation {

Navigation::Navigation(Logger &log, uint32_t axis /*=0*/)
    : log_(log),
      data_(Data::getInstance()),
      status_(ModuleStatus::kStart),
      log_counter_(0),
      movement_axis_(axis),
      calibration_limits_{{0.05, 0.05, 0.05}},
      current_measurements_(0),
      is_imu_reliable_{{true, true, true, true}},
      num_outlier_imus_(0),
      acceleration_(0, 0.),
      velocity_(0, 0.),
      displacement_(0, 0.),
      displacement_uncertainty_(0.),
      velocity_uncertainty_(0.),
      has_initial_time_(false),
      stripe_counter_(log_, data_, displacement_uncertainty_, velocity_uncertainty_,
                      kStripeDistance),
      is_keyence_used_(true),
      is_keyence_real_(true),
      acceleration_integrator_(&velocity_),
      velocity_integrator_(&displacement_)
{
  log_.INFO("NAV", "Navigation module started");
  for (uint32_t i = 0; i < Sensors::kNumImus; i++) {
    filters_[i] = KalmanFilter(1, 1);
    filters_[i].setup();
  }
  status_ = ModuleStatus::kInit;
  updateData();
  log_.INFO("NAV", "Navigation module initialised");
}

ModuleStatus Navigation::getModuleStatus() const
{
  return status_;
}

data::nav_t Navigation::getAcceleration() const
{
  return acceleration_.value;
}

data::nav_t Navigation::getVelocity() const
{
  return velocity_.value;
}

data::nav_t Navigation::getDisplacement() const
{
  return displacement_.value;
}

data::nav_t Navigation::getEmergencyBrakingDistance() const
{
  // TODO(Anyone): Account for actuation delay and/or communication latency?
  // Also, how realistic is this? (e.g brake force wearing down)
  return getVelocity() * getVelocity() / (2 * kEmergencyDeceleration);
}

data::nav_t Navigation::getBrakingDistance() const
{
  const auto motor_data = data_.getMotorData();
  uint32_t total_rpm    = 0;
  for (uint32_t i = 0; i < data::Motors::kNumMotors; i++) {
    total_rpm += motor_data.rpms[i];
  }
  const auto avg_rpm
    = static_cast<data::nav_t>(total_rpm) / static_cast<data::nav_t>(data::Motors::kNumMotors);
  const auto rot_velocity = (avg_rpm / 60.0) * (2 * kPi);

  const auto actuation_force = kSpringCompression * kSpringCoefficient;
  const auto braking_force
    = (actuation_force * kFrictionCoefficient) / (std::tan(kEmbrakeAngle) - kFrictionCoefficient);
  const auto deceleration_total = static_cast<data::nav_t>(kNumBrakes) * braking_force / kPodMass;

  const auto velocity           = getVelocity();
  const auto pod_kinetic_energy = 0.5 * kPodMass * velocity;
  const auto rotational_kinetic_energy
    = data::Motors::kNumMotors * 0.5 * kMomentOfInertiaWheel * rot_velocity * rot_velocity;
  const auto total_kinetic_energy = pod_kinetic_energy + rotational_kinetic_energy;

  const auto braking_distance = (total_kinetic_energy / kPodMass) / deceleration_total;

  return braking_distance;
}

Navigation::NavigationVectorArray Navigation::getGravityCalibration() const
{
  return gravity_calibration_;
}

void Navigation::calibrateGravity()
{
  log_.INFO("NAV", "Calibrating gravity");
  std::array<utils::math::RollingStatistics<data::NavigationVector>, Sensors::kNumImus> online_array
    = {{utils::math::RollingStatistics<data::NavigationVector>(kCalibrationQueries),
        utils::math::RollingStatistics<data::NavigationVector>(kCalibrationQueries),
        utils::math::RollingStatistics<data::NavigationVector>(kCalibrationQueries),
        utils::math::RollingStatistics<data::NavigationVector>(kCalibrationQueries)}};
  bool calibration_successful   = false;
  uint32_t calibration_attempts = 0;

  while (!calibration_successful && calibration_attempts < kMaxCalibrationAttempts) {
    log_.INFO("NAV", "Calibration attempt %d", calibration_attempts + 1);

    // Average each sensor over specified number of readings
    for (uint32_t i = 0; i < kCalibrationQueries; ++i) {
      const auto full_imu_data = data_.getSensorsImuData();
      for (uint32_t j = 0; j < data::Sensors::kNumImus; ++j) {
        const auto single_imu_data = full_imu_data.value[j].acc;
        online_array[j].update(single_imu_data);
        if (write_to_file_) {
          std::ofstream output_file;
          output_file.open("src/navigation/testing/nav_data.csv", std::ios::app);
          output_file << j << kDelimiter;
          output_file << single_imu_data[0] << kDelimiter;
          output_file << single_imu_data[1] << kDelimiter;
          output_file << single_imu_data[2] << std::endl;
          output_file.close();
        }
      }
      utils::concurrent::Thread::sleep(1);
    }
    // Check if each calibration's variance is acceptable
    calibration_successful = true;
    for (uint32_t i = 0; i < data::Sensors::kNumImus; ++i) {
      for (uint32_t j = 0; j < 3; ++j) {
        const bool var_within_lim = online_array[i].getVariance()[j] < calibration_limits_[j];
        calibration_successful    = calibration_successful && var_within_lim;
      }
    }
    calibration_attempts++;
  }

  // Store calibration and update filters if successful
  if (calibration_successful) {
    log_.INFO("NAV", "Calibration of IMU acceleration succeeded with final readings:");
    for (uint32_t i = 0; i < data::Sensors::kNumImus; ++i) {
      gravity_calibration_[i] = online_array[i].getMean();
      data::nav_t variance    = 0.0;
      for (uint32_t j = 0; j < 3; ++j) {
        variance += online_array[i].getVariance()[j];
      }
      filters_[i].updateMeasurementCovarianceMatrix(variance);

      log_.INFO("NAV", "\tIMU %d: g=(%.5f, %.5f, %.5f), variance=%.5f", i,
                gravity_calibration_[i][0], gravity_calibration_[i][1], gravity_calibration_[i][2],
                variance);
    }
    // set calibration uncertainties
    for (uint32_t axis = 0; axis < 3; axis++) {
      for (uint32_t i = 0; i < data::Sensors::kNumImus; i++) {
        const auto variance = online_array[i].getVariance()[axis];
        calibration_variance_[axis] += variance * variance;
      }
      // geometric mean for variances of different IMUs
      calibration_variance_[axis] = std::sqrt(calibration_variance_[axis]);
    }
    log_.INFO("NAV", "Calibration Variance: x-axis: %.3f, y-axis: %.3f, z-axis: %.3f",
              calibration_variance_[0], calibration_variance_[1], calibration_variance_[2]);
    status_ = data::ModuleStatus::kReady;
    updateData();
    log_.INFO("NAV", "Navigation module ready");
  } else {
    log_.INFO("NAV", "Calibration of IMU acceleration failed with final readings:");
    for (uint32_t i = 0; i < Sensors::kNumImus; ++i) {
      const auto acceleration = online_array[i].getMean();
      data::nav_t variance    = 0.0;
      for (uint32_t j = 0; j < 3; ++j) {
        variance += online_array[i].getVariance()[j];
      }

      log_.INFO("NAV", "\tIMU %d: g=(%.5f, %.5f, %.5f), variance=%.5f", i, acceleration[0],
                acceleration[1], acceleration[2], variance);
    }
    status_ = ModuleStatus::kCriticalFailure;
    updateData();
    log_.ERR("NAV", "Navigation module failed on calibration");
  }
}

void Navigation::queryImus()
{
  NavigationArray raw_acceleration_moving;  // Raw values in moving axis

  const auto imu_data = data_.getSensorsImuData();
  uint32_t t          = imu_data.timestamp;
  // process raw values
  ImuAxisData raw_acceleration;  // All raw data, four values per axis
  for (uint32_t axis = 0; axis < 3; axis++) {
    for (uint32_t i = 0; i < Sensors::kNumImus; ++i) {
      const auto acceleration   = imu_data.value[i].acc - gravity_calibration_[i];
      raw_acceleration[axis][i] = acceleration[axis];

      // the moving axis should be set to 0 for tukeyFences
      if (!is_imu_reliable_[i]) {
        raw_acceleration_moving[i] = 0;
      } else if (axis == movement_axis_) {
        raw_acceleration_moving[i] = acceleration[movement_axis_];
      }
    }
  }
  log_.DBG1("NAV", "Raw acceleration values: %.3f, %.3f, %.3f, %.3f", raw_acceleration_moving[0],
            raw_acceleration_moving[1], raw_acceleration_moving[2], raw_acceleration_moving[3]);
  // Run outlier detection on moving axis
  tukeyFences(raw_acceleration_moving, kTukeyThreshold);
  // TODO(Justus) how to run outlier detection on non-moving axes without affecting "reliable"
  // Current idea: outlier function takes reliability write flag, on hold until z-score impl.

  // Kalman filter the readings which are reliable
  utils::math::OnlineStatistics<data::nav_t> acceleration_average_filter;
  for (uint32_t i = 0; i < Sensors::kNumImus; ++i) {
    if (is_imu_reliable_[i]) {
      data::nav_t estimate = filters_[i].filter(raw_acceleration_moving[i]);
      acceleration_average_filter.update(estimate);
    }
  }
  previous_measurements_[current_measurements_] = raw_acceleration;
  current_measurements_++;
  if (current_measurements_ == kPreviousMeasurements) {
    current_measurements_ = 0;
    previous_filled_      = 1;
  }
  if (previous_filled_) checkVibration();

  acceleration_.value     = acceleration_average_filter.getMean();
  acceleration_.timestamp = t;

  acceleration_integrator_.update(acceleration_);
  velocity_integrator_.update(velocity_);
}

void Navigation::checkVibration()
{
  // curr_msmt points at next measurement, ie the last one
  std::array<utils::math::OnlineStatistics<data::nav_t>, 3> online_array_axis;
  for (uint32_t i = 0; i < kPreviousMeasurements; i++) {
    const auto raw_data
      = previous_measurements_[(current_measurements_ + i) % kPreviousMeasurements];
    for (uint32_t axis = 0; axis < 3; axis++) {
      if (axis != movement_axis_) {  // assume variance in moving axis are not vibrations
        for (uint32_t imu = 0; imu < Sensors::kNumImus; imu++) {
          online_array_axis[axis].update(raw_data[axis][imu]);
        }
      }
    }
  }
  for (uint32_t axis = 0; axis < 3; axis++) {
    const auto variance = online_array_axis[axis].getVariance();
    if (log_counter_ % 100000 == 0 && axis != movement_axis_) {
      log_.INFO("NAV", "Variance in axis %d: %.3f", axis, variance);
    }
    const auto ratio                      = variance / calibration_variance_[axis];
    const auto statistical_variance_ratio = static_cast<data::nav_t>(kCalibrationQueries)
                                            / static_cast<data::nav_t>(kPreviousMeasurements);
    if (ratio > statistical_variance_ratio) {
      log_.ERR("NAV", "Variance in axis %d is %.3f times larger than its calibration variance.",
               axis, ratio);
    }
  }
}

void Navigation::updateUncertainty()
{
  const auto time_delta
    = static_cast<data::nav_t>(displacement_.timestamp - previous_timestamp_) / 1000000.0;
  const auto absolute_acceleration_delta = std::abs(getAcceleration() - previous_acceleration_);
  // Random walk uncertainty
  velocity_uncertainty_ += absolute_acceleration_delta * time_delta / 2.;
  // Processing uncertainty
  data::nav_t acceleration_variance_ = 0.0;
  for (uint32_t i = 0; i < Sensors::kNumImus; i++) {
    acceleration_variance_ += filters_[i].getEstimateVariance();
  }
  acceleration_variance_                     = acceleration_variance_ / data::Sensors::kNumImus;
  const auto acceleration_standard_deviation = std::sqrt(acceleration_variance_);
  // uncertainty in velocity is the std deviation of acceleration integrated
  velocity_uncertainty_ += acceleration_standard_deviation * time_delta;
  displacement_uncertainty_ += velocity_uncertainty_ * time_delta;
  // Random walk uncertainty
  displacement_uncertainty_ += std::abs(getVelocity() - previous_velocity_) * time_delta / 2.;
}

void Navigation::disableKeyenceUsage()
{
  is_keyence_used_ = false;
}

void Navigation::setKeyenceFake()
{
  is_keyence_real_ = false;
}

bool Navigation::getHasInit()
{
  return has_initial_time_;
}

void Navigation::setHasInit()
{
  has_initial_time_ = true;
}

void Navigation::logWrite()
{
  write_to_file_ = true;
}

void Navigation::tukeyFences(NavigationArray &data_array, const data::nav_t threshold)
{
  // Define the quartiles first:
  data::nav_t q1 = 0;
  data::nav_t q2 = 0;
  data::nav_t q3 = 0;
  // The most likely case is that all four IMUs are still reliable:
  if (num_outlier_imus_ == 0) {
    // copy the original array for sorting
    NavigationArray data_array_sorted = data_array;
    std::sort(data_array_sorted.begin(), data_array_sorted.end());
    // find the quartiles
    q1 = (data_array_sorted[0] + data_array_sorted[1]) / 2.;
    q2 = (data_array_sorted[1] + data_array_sorted[2]) / 2.;
    q3 = (data_array_sorted[2] + data_array_sorted[3]) / 2.;
    // The second case is that one IMU is faulty
  } else if (num_outlier_imus_ == 1) {
    // select non-outlier values
    NavigationArrayOneFaulty data_array_faulty;
    if (!is_imu_reliable_[0]) {
      data_array_faulty = {{data_array[1], data_array[2], data_array[3]}};
    } else if (!is_imu_reliable_[1]) {
      data_array_faulty = {{data_array[0], data_array[2], data_array[3]}};
    } else if (!is_imu_reliable_[2]) {
      data_array_faulty = {{data_array[0], data_array[1], data_array[3]}};
    } else if (!is_imu_reliable_[3]) {
      data_array_faulty = {{data_array[0], data_array[1], data_array[2]}};
    }
    std::sort(data_array_faulty.begin(), data_array_faulty.end());
    q1 = (data_array_faulty[0] + data_array_faulty[1]) / 2.;
    q2 = data_array_faulty[1];
    q3 = (data_array_faulty[1] + data_array_faulty[2]) / 2.;
  } else if (num_outlier_imus_ < 4) {
    // set all 0.0 IMUs to non-zero avg
    data::nav_t sum_non_outliers = 0.0;
    uint32_t num_non_outliers    = 0;
    for (uint32_t i = 0; i < Sensors::kNumImus; ++i) {
      if (data_array[i] != 0.0) {
        // no outlier
        num_non_outliers += 1;
        sum_non_outliers += data_array[i];
      }
    }
    for (uint32_t i = 0; i < Sensors::kNumImus; ++i) {
      data_array[i] = sum_non_outliers / num_non_outliers;
    }
    // do not check for further outliers because no reliable detection could be made!
    return;
  }
  // find the thresholds
  // clip IQR to upper bound to avoid issues with very large outliers
  const auto iqr         = std::min(q3 - q1, kTukeyIQRBound);
  const auto upper_limit = q3 + threshold * iqr;
  const auto lower_limit = q1 - threshold * iqr;
  // replace any outliers with the median
  for (uint32_t i = 0; i < Sensors::kNumImus; ++i) {
    const auto exceeds_limits = data_array[i] < lower_limit || data_array[i] > upper_limit;
    if (exceeds_limits && is_imu_reliable_[i]) {
      log_.DBG3(
        "NAV",
        "Outlier detected in IMU %d, reading: %.3f not in [%.3f, %.3f]. Updated to %.3f",  // NOLINT
        i + 1, data_array[i], lower_limit, upper_limit, q2);

      data_array[i] = q2;
      imu_outlier_counter_[i]++;
      // If this counter exceeds some threshold then that IMU is deemed unreliable
      if (imu_outlier_counter_[i] > 1000 && is_imu_reliable_[i]) {
        is_imu_reliable_[i] = false;
        num_outlier_imus_++;
      }
      if (num_outlier_imus_ > 1) {
        status_ = ModuleStatus::kCriticalFailure;
        log_.ERR("NAV", "At least two IMUs no longer reliable, entering CriticalFailure.");
      }
    } else {
      imu_outlier_counter_[i] = 0;
    }
  }
}

void Navigation::updateData()
{
  data::Navigation nav_data;
  nav_data.module_status              = getModuleStatus();
  nav_data.displacement               = getDisplacement();
  nav_data.velocity                   = getVelocity();
  nav_data.acceleration               = getAcceleration();
  nav_data.emergency_braking_distance = getEmergencyBrakingDistance();
  nav_data.braking_distance           = 1.2 * getEmergencyBrakingDistance();

  data_.setNavigationData(nav_data);

  if (log_counter_ % 100 == 0) {                                                 // kPrintFreq
    log_.DBG("NAV", "%d: Data Update: a=%.3f, v=%.3f, d=%.3f, d(keyence)=%.3f",  // NOLINT
             log_counter_, nav_data.acceleration, nav_data.velocity, nav_data.displacement,
             stripe_counter_.getStripeCount() * kStripeDistance);
    log_.DBG("NAV", "%d: Data Update: v(unc)=%.3f, d(unc)=%.3f, keyence failures: %d", log_counter_,
             velocity_uncertainty_, displacement_uncertainty_, stripe_counter_.getFailureCount());
  }
  log_counter_++;
  // Update all prev measurements
  previous_timestamp_    = displacement_.timestamp;
  previous_acceleration_ = getAcceleration();
  previous_velocity_     = getVelocity();
}

void Navigation::navigate()
{
  queryImus();
  if (is_keyence_used_) {
    stripe_counter_.queryKeyence(displacement_.value, velocity_.value, is_keyence_real_);
    if (stripe_counter_.checkFailure(displacement_.value)) status_ = ModuleStatus::kCriticalFailure;
  }
  if (log_counter_ > 1000) updateUncertainty();
  updateData();
}

void Navigation::initialiseTimestamps()
{
  // First iteration --> set timestamps
  acceleration_.timestamp = utils::Timer::getTimeMicros();
  velocity_.timestamp     = utils::Timer::getTimeMicros();
  displacement_.timestamp = utils::Timer::getTimeMicros();
  previous_acceleration_  = getAcceleration();
  previous_velocity_      = getVelocity();
  initial_timestamp_      = utils::Timer::getTimeMicros();
  log_.DBG3("NAV", "Initial timestamp:%d", initial_timestamp_);
  previous_timestamp_ = utils::Timer::getTimeMicros();
  stripe_counter_.set_init(initial_timestamp_);
}
}  // namespace navigation
}  // namespace hyped
