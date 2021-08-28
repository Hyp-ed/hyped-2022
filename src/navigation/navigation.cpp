#include "navigation.hpp"

#include <algorithm>

#include <utils/concurrent/thread.hpp>
#include <utils/timer.hpp>

namespace hyped {

using hyped::utils::concurrent::Thread;

namespace navigation {

Navigation::Navigation(Logger &log, unsigned int axis /*=0*/)
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
  for (unsigned int i = 0; i < Sensors::kNumImus; i++) {
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
  data::Motors motor_data = data_.getMotorData();
  uint32_t total_rpm      = 0;
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    total_rpm += motor_data.rpms[i];
  }
  uint32_t avg_rpm         = total_rpm / data::Motors::kNumMotors;
  data::nav_t rot_velocity = (avg_rpm / 60) * (2 * kPi);

  data::nav_t actuation_force = kSpringCompression * kSpringCoefficient;
  data::nav_t braking_force
    = (actuation_force * kFrictionCoefficient) / (tan(kEmbrakeAngle) - kFrictionCoefficient);
  data::nav_t deceleration_total = kNumBrakes * braking_force / kPodMass;

  data::nav_t pod_kinetic_energy = 0.5 * kPodMass * getVelocity() * getVelocity();
  data::nav_t rotational_kinetic_energy
    = data::Motors::kNumMotors * 0.5 * kMomentOfInertiaWheel * rot_velocity * rot_velocity;
  data::nav_t total_kinetic_energy = pod_kinetic_energy + rotational_kinetic_energy;

  data::nav_t braking_distance = (total_kinetic_energy / kPodMass) / deceleration_total;

  return braking_distance;
}

Navigation::NavigationVectorArray Navigation::getGravityCalibration() const
{
  return gravity_calibration_;
}

void Navigation::calibrateGravity()
{
  log_.INFO("NAV", "Calibrating gravity");
  std::array<utils::math::RollingStatistics<NavigationVector>, Sensors::kNumImus> online_array
    = {{utils::math::RollingStatistics<NavigationVector>(kCalibrationQueries),
        utils::math::RollingStatistics<NavigationVector>(kCalibrationQueries),
        utils::math::RollingStatistics<NavigationVector>(kCalibrationQueries),
        utils::math::RollingStatistics<NavigationVector>(kCalibrationQueries)}};
  bool calibration_successful = false;
  int calibration_attempts    = 0;

  while (!calibration_successful && calibration_attempts < kMaxCalibrationAttempts) {
    log_.INFO("NAV", "Calibration attempt %d", calibration_attempts + 1);

    // Average each sensor over specified number of readings
    for (int i = 0; i < kCalibrationQueries; ++i) {
      sensor_readings_ = data_.getSensorsImuData();
      for (int j = 0; j < data::Sensors::kNumImus; ++j) {
        NavigationVector imu_data = sensor_readings_.value[j].acc;
        online_array[j].update(imu_data);
        if (write_to_file_) {
          std::ofstream writefile;
          writefile.open("src/navigation/testing/nav_data.csv", std::ios::app);
          writefile << j << kDelimiter << imu_data[0] << kDelimiter << imu_data[1] << kDelimiter
                    << imu_data[2] << std::endl;
          writefile.close();
        }
      }
      Thread::sleep(1);
    }
    // Check if each calibration's variance is acceptable
    calibration_successful = true;
    for (int i = 0; i < data::Sensors::kNumImus; ++i) {
      for (int j = 0; j < 3; ++j) {
        bool var_within_lim    = online_array[i].getVariance()[j] < calibration_limits_[j];
        calibration_successful = calibration_successful && var_within_lim;
      }
    }
    calibration_attempts++;
  }

  // Store calibration and update filters if successful
  if (calibration_successful) {
    log_.INFO("NAV", "Calibration of IMU acceleration succeeded with final readings:");
    for (int i = 0; i < data::Sensors::kNumImus; ++i) {
      gravity_calibration_[i] = online_array[i].getMean();
      double variance         = 0.0;
      for (int j = 0; j < 3; ++j) {
        variance += online_array[i].getVariance()[j];
      }
      filters_[i].updateMeasurementCovarianceMatrix(variance);

      log_.INFO("NAV", "\tIMU %d: g=(%.5f, %.5f, %.5f), variance=%.5f", i,
                gravity_calibration_[i][0], gravity_calibration_[i][1], gravity_calibration_[i][2],
                variance);
    }
    // set calibration uncertainties
    for (int axis = 0; axis < 3; axis++) {
      for (int i = 0; i < data::Sensors::kNumImus; i++) {
        double variance = (online_array[i].getVariance()[axis]);
        calibration_variance_[axis] += variance * variance;
      }
      // geometric mean for variances of different IMUs
      calibration_variance_[axis] = sqrt(calibration_variance_[axis]);
    }
    log_.INFO("NAV", "Calibration Variance: x-axis: %.3f, y-axis: %.3f, z-axis: %.3f",
              calibration_variance_[0], calibration_variance_[1], calibration_variance_[2]);
    status_ = data::ModuleStatus::kReady;
    updateData();
    log_.INFO("NAV", "Navigation module ready");
  } else {
    log_.INFO("NAV", "Calibration of IMU acceleration failed with final readings:");
    for (int i = 0; i < Sensors::kNumImus; ++i) {
      NavigationVector acc = online_array[i].getMean();
      double variance      = 0.0;
      for (int j = 0; j < 3; ++j) {
        variance += online_array[i].getVariance()[j];
      }

      log_.INFO("NAV", "\tIMU %d: g=(%.5f, %.5f, %.5f), variance=%.5f", i, acc[0], acc[1], acc[2],
                variance);
    }
    status_ = ModuleStatus::kCriticalFailure;
    updateData();
    log_.ERR("NAV", "Navigation module failed on calibration");
  }
}

data::nav_t Navigation::accNorm(NavigationVector &acc)
{
  data::nav_t norm = 0.0;
  for (unsigned int i = 0; i < 3; i++) {
    data::nav_t a = acc[i];
    norm += a * a;
  }
  norm = std::sqrt(norm);
  return norm;
}

void Navigation::queryImus()
{
  ImuAxisData raw_acceleration;             // All raw data, four values per axis
  NavigationArray raw_acceleration_moving;  // Raw values in moving axis

  utils::math::OnlineStatistics<data::nav_t> acceleration_average_filter;
  sensor_readings_ = data_.getSensorsImuData();
  uint32_t t       = sensor_readings_.timestamp;
  // process raw values
  for (uint8_t axis = 0; axis < 3; axis++) {
    for (int i = 0; i < Sensors::kNumImus; ++i) {
      NavigationVector acceleration = sensor_readings_.value[i].acc - gravity_calibration_[i];
      raw_acceleration[axis][i]     = acceleration[axis];

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

  /*for (int axis = 0; axis < 3; axis++) {
    if (axis != movement_axis_) tukeyFences(raw_acceleration[axis], kTukeyThreshold);
  }*/

  // Kalman filter the readings which are reliable
  for (int i = 0; i < Sensors::kNumImus; ++i) {
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
  for (int i = 0; i < kPreviousMeasurements; i++) {
    ImuAxisData raw_data
      = previous_measurements_[(current_measurements_ + i) % kPreviousMeasurements];
    for (uint8_t axis = 0; axis < 3; axis++) {
      if (axis != movement_axis_) {  // assume variance in moving axis are not vibrations
        for (int imu = 0; imu < Sensors::kNumImus; imu++) {
          online_array_axis[axis].update(raw_data[axis][imu]);
        }
      }
    }
  }
  for (uint8_t axis = 0; axis < 3; axis++) {
    double variance = online_array_axis[axis].getVariance();
    if (log_counter_ % 100000 == 0 && axis != movement_axis_) {
      log_.INFO("NAV", "Variance in axis %d: %.3f", axis, variance);
    }
    double ratio                      = variance / calibration_variance_[axis];
    double statistical_variance_ratio = kCalibrationQueries / kPreviousMeasurements;
    if (ratio > statistical_variance_ratio) {
      log_.ERR("NAV", "Variance in axis %d is %.3f times larger than its calibration variance.",
               axis, ratio);
    }
  }
}

void Navigation::updateUncertainty()
{
  double delta_t            = (displacement_.timestamp - previous_timestamp_) / 1000000.0;
  data::nav_t abs_delta_acc = abs(getAcceleration() - previous_acceleration_);
  // Random walk uncertainty
  velocity_uncertainty_ += abs_delta_acc * delta_t / 2.;
  // Processing uncertainty
  data::nav_t acceleration_variance_ = 0.0;
  for (int i = 0; i < Sensors::kNumImus; i++) {
    acceleration_variance_ += filters_[i].KalmanFilter::getEstimateVariance();
  }
  acceleration_variance_                      = acceleration_variance_ / data::Sensors::kNumImus;
  data::nav_t acceleration_standard_deviation = std::sqrt(acceleration_variance_);
  // uncertainty in velocity is the std deviation of acceleration integrated
  velocity_uncertainty_ += acceleration_standard_deviation * delta_t;
  displacement_uncertainty_ += velocity_uncertainty_ * delta_t;
  // Random walk uncertainty
  displacement_uncertainty_ += abs(getVelocity() - previous_velocity_) * delta_t / 2.;
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

void Navigation::tukeyFences(NavigationArray &data_array, data::nav_t threshold)
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
    data::nav_t sum_non_outliers  = 0.0;
    unsigned int num_non_outliers = 0;
    for (int i = 0; i < Sensors::kNumImus; ++i) {
      if (data_array[i] != 0.0) {
        // no outlier
        num_non_outliers += 1;
        sum_non_outliers += data_array[i];
      }
    }
    for (int i = 0; i < Sensors::kNumImus; ++i) {
      data_array[i] = sum_non_outliers / num_non_outliers;
    }
    // do not check for further outliers because no reliable detection could be made!
    return;
  }
  // find the thresholds
  data::nav_t iqr = q3 - q1;
  // clip IQR to upper bound to avoid issues with very large outliers
  if (iqr > kTukeyIQRBound) { iqr = kTukeyIQRBound; }
  data::nav_t upper_limit = q3 + threshold * iqr;
  data::nav_t lower_limit = q1 - threshold * iqr;
  // replace any outliers with the median
  for (int i = 0; i < Sensors::kNumImus; ++i) {
    if ((data_array[i] < lower_limit or data_array[i] > upper_limit) && is_imu_reliable_[i]) {
      log_.DBG3(
        "NAV",
        "Outlier detected in IMU %d, reading: %.3f not in [%.3f, %.3f]. Updated to %.3f",  // NOLINT
        i + 1, data_array[i], lower_limit, upper_limit, q2);
      // log_.DBG3("NAV", "Outlier detected with quantiles: %.3f, %.3f, %.3f", q1, q2, q3);

      data_array[i] = q2;
      imu_outlier_counter_[i]++;
      // If this counter exceeds some threshold then that IMU is deemed unreliable
      if (imu_outlier_counter_[i] > 1000 && is_imu_reliable_[i]) {
        // log_.DBG3("NAV", "IMU%d is an outlier!", i + 1);
        is_imu_reliable_[i] = false;
        num_outlier_imus_++;
      }
      if (num_outlier_imus_ > 1) {
        status_ = ModuleStatus::kCriticalFailure;
        log_.ERR("NAV", "At least two IMUs no longer reliable, entering CriticalFailure.");
      }
    } else {
      imu_outlier_counter_[i] = 0;
      if (log_counter_ % 100 == 0 && is_imu_reliable_[i]) {
        /*
         * log_.DBG3("NAV", "No Outlier detected in IMU %d, reading: %.3f in [%.3f, %.3f]",
         *           i+1, data_array[i], lower_limit, upper_limit);
         */
      }
    }
  }
  /*
   * if (log_counter_ % 100 == 0) {
   *   log_.DBG3("NAV", "Outliers: IMU1: %d, IMU2: %d, IMU3: %d, IMU4: %d", imu_outlier_counter_[0],
   *    imu_outlier_counter_[1], imu_outlier_counter_[2], imu_outlier_counter_[3]);
   *   log_.DBG3("NAV", "Number of outliers: %d", nOutlierImus_);
   * }
   */
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
