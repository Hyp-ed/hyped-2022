#include "navigation.hpp"

#include <algorithm>
#include <vector>

#include <utils/concurrent/thread.hpp>
#include <utils/timer.hpp>

namespace hyped::navigation {

Navigation::Navigation(const std::uint32_t axis /*=0*/)
    : data_(data::Data::getInstance()),
      log_("NAVIGATION", utils::System::getSystem().config_.log_level_navigation),
      status_(data::ModuleStatus::kStart),
      log_counter_(0),
      movement_axis_(axis),
      calibration_limits_{{0.05, 0.05, 0.05}},
      current_measurements_(0),
      is_imu_reliable_{{true, true, true, true}},
      num_outlier_imus_(0),
      imu_outlier_counter_{{0, 0, 0, 0}},
      is_encoder_reliable_{{true, true, true, true}},
      num_outlier_encoders_(0),
      encoder_outlier_counter_{{0, 0, 0, 0}},
      acceleration_(0, 0.),
      velocity_(0, 0.),
      displacement_(0, 0.),
      displacement_uncertainty_(0.),
      velocity_uncertainty_(0.),
      has_initial_time_(false),
      acceleration_integrator_(&velocity_),
      velocity_integrator_(&displacement_)
{
  log_.info("Navigation module started");
  for (std::size_t i = 0; i < data::Sensors::kNumImus; ++i) {
    filters_[i] = KalmanFilter(1, 1);
    filters_[i].setup();
  }
  status_ = data::ModuleStatus::kInit;
  updateData();
  log_.info("Navigation module initialised");
}

data::nav_t Navigation::getEncoderDisplacement() const
{
  return encoder_displacement_.value;
}

data::ModuleStatus Navigation::getModuleStatus() const
{
  return status_;
}

data::nav_t Navigation::getImuAcceleration() const
{
  return acceleration_.value;
}

data::nav_t Navigation::getImuVelocity() const
{
  return velocity_.value;
}

data::nav_t Navigation::getImuDisplacement() const
{
  return displacement_.value;
}

data::nav_t Navigation::getEmergencyBrakingDistance() const
{
  // TODO(Anyone): Account for actuation delay and/or communication latency?
  // Also, how realistic is this? (e.g brake force wearing down)
  return getImuVelocity() * getImuVelocity() / (2 * kEmergencyDeceleration);
}

data::nav_t Navigation::getBrakingDistance() const
{
  const auto motor_data = data_.getMotorData();

  const uint32_t total_rpm = std::accumulate(motor_data.rpms.begin(), motor_data.rpms.end(), 0);
  const auto avg_rpm
    = static_cast<data::nav_t>(total_rpm) / static_cast<data::nav_t>(data::Motors::kNumMotors);
  const auto rot_velocity = (avg_rpm / 60.0) * (2 * data::Navigation::kPi);

  const auto actuation_force = kSpringCompression * kSpringCoefficient;
  const auto braking_force
    = (actuation_force * kFrictionCoefficient) / (std::tan(kEmbrakeAngle) - kFrictionCoefficient);
  const auto deceleration_total = static_cast<data::nav_t>(kNumBrakes) * braking_force / kPodMass;

  const auto velocity           = getImuVelocity();
  const auto pod_kinetic_energy = 0.5 * kPodMass * velocity * velocity;
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
  log_.info("Calibrating gravity");
  std::array<utils::math::RollingStatistics<data::NavigationVector>, data::Sensors::kNumImus>
    online_array = {{utils::math::RollingStatistics<data::NavigationVector>(kCalibrationQueries),
                     utils::math::RollingStatistics<data::NavigationVector>(kCalibrationQueries),
                     utils::math::RollingStatistics<data::NavigationVector>(kCalibrationQueries),
                     utils::math::RollingStatistics<data::NavigationVector>(kCalibrationQueries)}};
  bool calibration_successful   = false;
  uint32_t calibration_attempts = 0;

  while (!calibration_successful && calibration_attempts < kMaxCalibrationAttempts) {
    log_.info("Calibration attempt %d", calibration_attempts + 1);

    // Average each sensor over specified number of readings
    for (std::size_t i = 0; i < kCalibrationQueries; ++i) {
      const auto full_imu_data = data_.getSensorsImuData();
      for (std::size_t j = 0; j < data::Sensors::kNumImus; ++j) {
        const auto single_imu_data = full_imu_data.value.at(j).acc;
        online_array.at(j).update(single_imu_data);
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
    for (const auto &online : online_array) {
      const auto variance = online.getVariance();
      for (std::size_t i = 0; i < 3; ++i) {
        const bool variance_within_limits = variance[i] < calibration_limits_.at(i);
        calibration_successful            = calibration_successful && variance_within_limits;
      }
    }
    calibration_attempts++;
  }

  // Store calibration and update filters if successful
  if (calibration_successful) {
    log_.info("Calibration of IMU acceleration succeeded with final readings:");
    for (std::size_t i = 0; i < data::Sensors::kNumImus; ++i) {
      gravity_calibration_[i]    = online_array[i].getMean();
      data::nav_t total_variance = 0.0;
      const auto variance        = online_array.at(i).getVariance();
      for (std::size_t j = 0; j < 3; ++j) {
        total_variance += variance[j];
      }
      filters_.at(i).updateMeasurementCovarianceMatrix(total_variance);

      log_.info("\tIMU %d: g=(%.5f, %.5f, %.5f), variance=%.5f", i, gravity_calibration_.at(i)[0],
                gravity_calibration_.at(i)[1], gravity_calibration_.at(i)[2], total_variance);
    }
    // set calibration uncertainties
    for (std::size_t axis = 0; axis < 3; ++axis) {
      for (std::size_t i = 0; i < data::Sensors::kNumImus; ++i) {
        const auto variance = online_array.at(i).getVariance()[axis];
        calibration_variance_.at(i) += variance * variance;
      }
      // geometric mean for variances of different IMUs
      calibration_variance_.at(axis) = std::sqrt(calibration_variance_.at(axis));
    }
    log_.info("Calibration Variance: x-axis: %.3f, y-axis: %.3f, z-axis: %.3f",
              calibration_variance_.at(0), calibration_variance_.at(1),
              calibration_variance_.at(2));
    status_ = data::ModuleStatus::kReady;
    updateData();
    log_.info("Navigation module ready");
  } else {
    log_.info("Calibration of IMU acceleration failed with final readings:");
    for (std::size_t i = 0; i < data::Sensors::kNumImus; ++i) {
      const auto acceleration    = online_array.at(i).getMean();
      const auto variance        = online_array.at(i).getVariance();
      data::nav_t total_variance = 0.0;
      for (std::size_t j = 0; j < 3; ++j) {
        total_variance += variance[j];
      }

      log_.info("\tIMU %d: g=(%.5f, %.5f, %.5f), variance=%.5f", i, acceleration[0],
                acceleration[1], acceleration[2], total_variance);
    }
    status_ = data::ModuleStatus::kCriticalFailure;
    updateData();
    log_.error("Navigation module failed on calibration");
  }
}

void Navigation::queryWheelEncoders()
{
  const auto encoder_data = data_.getSensorsWheelEncoderData();

  EncoderArray encoder_data_array;
  uint32_t sum = 0;
  for (size_t i = 0; i < encoder_data.size(); ++i) {
    sum += encoder_data.at(i).value;
    encoder_data_array.at(i) = encoder_data.at(i).value;
  }

  const data::nav_t average   = static_cast<data::nav_t>(sum / encoder_data.size());
  encoder_displacement_.value = average * data::Navigation::kWheelCircumfrence;

  wheelEncoderOutlierDetection(encoder_data_array);
}

void Navigation::queryImus()
{
  NavigationArray raw_acceleration_moving;  // Raw values in moving axis

  const auto imu_data                      = data_.getSensorsImuData();
  const uint64_t current_trajectory_micros = imu_data.timestamp;
  // process raw values
  ImuAxisData raw_acceleration;  // All raw data, four values per axis
  for (std::size_t i = 0; i < data::Sensors::kNumImus; ++i) {
    const auto acceleration = imu_data.value.at(i).acc - gravity_calibration_.at(i);
    for (std::size_t axis = 0; axis < 3; ++axis) {
      raw_acceleration.at(axis)[i] = acceleration[axis];
    }
    // the moving axis should be set to 0 for outlier detection
    if (!is_imu_reliable_.at(i)) { raw_acceleration_moving.at(i) = 0; }
    raw_acceleration_moving.at(i) = acceleration[movement_axis_];
  }
  log_.debug("Raw acceleration values: %.3f, %.3f, %.3f, %.3f", raw_acceleration_moving[0],
             raw_acceleration_moving[1], raw_acceleration_moving[2], raw_acceleration_moving[3]);
  // Run outlier detection on moving axis
  imuOutlierDetection(raw_acceleration_moving);
  // TODO(Justus) how to run outlier detection on non-moving axes without affecting "reliable"
  // Current idea: outlier function takes reliability write flag, on hold until z-score impl.

  // Kalman filter the readings which are reliable
  utils::math::OnlineStatistics<data::nav_t> acceleration_average_filter;
  for (std::size_t i = 0; i < data::Sensors::kNumImus; ++i) {
    if (is_imu_reliable_.at(i)) {
      data::nav_t estimate = filters_.at(i).filter(raw_acceleration_moving.at(i));
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
  acceleration_.timestamp = current_trajectory_micros;

  acceleration_integrator_.update(acceleration_);
  velocity_integrator_.update(velocity_);
}

void Navigation::compareEncoderImu()
{
  const data::nav_t encoder_displacement = getEncoderDisplacement();
  const data::nav_t imu_displacement     = getImuDisplacement();
  const data::nav_t imu_encoder_error    = std::abs(encoder_displacement - imu_displacement);

  if (imu_encoder_error > data::Navigation::kImuEncoderMaxError) {
    auto navigation_data          = data_.getNavigationData();
    navigation_data.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setNavigationData(navigation_data);
    log_.error("Wheel encoder data disagrees with IMU data, entering kCriticalFailure");
  }
}

void Navigation::checkVibration()
{
  // curr_msmt points at next measurement, ie the last one
  std::array<utils::math::OnlineStatistics<data::nav_t>, 3> online_array_axis;
  for (std::size_t i = 0; i < kPreviousMeasurements; ++i) {
    const auto raw_data
      = previous_measurements_[(current_measurements_ + i) % kPreviousMeasurements];
    for (std::size_t axis = 0; axis < 3; ++axis) {
      if (axis != movement_axis_) {  // assume variance in moving axis are not vibrations
        for (const auto raw_data_for_imu : raw_data[axis]) {
          online_array_axis[axis].update(raw_data_for_imu);
        }
      }
    }
  }
  for (std::size_t axis = 0; axis < 3; ++axis) {
    const auto variance = online_array_axis[axis].getVariance();
    if (log_counter_ % 100000 == 0 && axis != movement_axis_) {
      log_.info("Variance in axis %d: %.3f", axis, variance);
    }
    const auto ratio                      = variance / calibration_variance_[axis];
    const auto statistical_variance_ratio = static_cast<data::nav_t>(kCalibrationQueries)
                                            / static_cast<data::nav_t>(kPreviousMeasurements);
    if (ratio > statistical_variance_ratio) {
      log_.error("Variance in axis %d is %.3f times larger than its calibration variance.", axis,
                 ratio);
    }
  }
}

void Navigation::updateUncertainty()
{
  // time difference in milliseconds
  const auto time_delta_secs
    = static_cast<data::nav_t>(displacement_.timestamp - previous_timestamp_) / 1e6;
  const auto absolute_acceleration_delta = std::abs(getImuAcceleration() - previous_acceleration_);
  // Random walk uncertainty
  velocity_uncertainty_ += absolute_acceleration_delta * time_delta_secs / 2.;
  // Processing uncertainty
  data::nav_t acceleration_variance_ = 0.0;
  for (auto &filter : filters_) {
    acceleration_variance_ += filter.getEstimateVariance();
  }
  acceleration_variance_                     = acceleration_variance_ / data::Sensors::kNumImus;
  const auto acceleration_standard_deviation = std::sqrt(acceleration_variance_);
  // uncertainty in velocity is the std deviation of acceleration integrated
  velocity_uncertainty_ += acceleration_standard_deviation * time_delta_secs;
  displacement_uncertainty_ += velocity_uncertainty_ * time_delta_secs;
  // Random walk uncertainty
  displacement_uncertainty_
    += std::abs(getImuVelocity() - previous_velocity_) * time_delta_secs / 2.;
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

Navigation::QuartileBounds Navigation::calculateImuQuartiles(const NavigationArray &data_array)
{
  std::vector<data::nav_t> data_vector;
  std::array<data::nav_t, 3> quartile_bounds;

  for (size_t i = 0; i < data::Sensors::kNumImus; ++i) {
    if (is_imu_reliable_.at(i)) { data_vector.push_back(data_array.at(i)); }
  }
  std::sort(data_vector.begin(), data_vector.end());

  quartile_bounds.at(0) = (data_vector.at(0) + data_vector.at(1)) / 2.;
  quartile_bounds.at(2)
    = (data_vector.at(data_vector.size() - 2) + data_vector.at(data_vector.size() - 1)) / 2.;
  if (num_outlier_imus_ == 0) {
    quartile_bounds.at(1) = (data_vector.at(1) + data_vector.at(2)) / 2.;
  } else if (num_outlier_imus_ == 1) {
    quartile_bounds.at(1) = data_vector.at(1);
  } else {
    auto navigation_data          = data_.getNavigationData();
    navigation_data.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setNavigationData(navigation_data);
    log_.error("At least two IMUs no longer reliable, entering CriticalFailure.");
  }
  return quartile_bounds;
}

Navigation::QuartileBounds Navigation::calculateEncoderQuartiles(const EncoderArray &data_array)
{
  std::vector<uint32_t> data_vector;

  for (size_t i = 0; i < data::Sensors::kNumEncoders; ++i) {
    if (is_encoder_reliable_.at(i)) { data_vector.push_back(data_array.at(i)); }
  }
  std::sort(data_vector.begin(), data_vector.end());

  std::array<data::nav_t, 3> quartile_bounds;
  quartile_bounds.at(0) = (data_vector.at(0) + data_vector.at(1)) / 2.;
  quartile_bounds.at(2)
    = (data_vector.at(data_vector.size() - 2) + data_vector.at(data_vector.size() - 1)) / 2.;
  if (num_outlier_encoders_ == 0) {
    quartile_bounds.at(1) = (data_vector.at(1) + data_vector.at(2)) / 2.;
  } else if (num_outlier_encoders_ == 1) {
    quartile_bounds.at(1) = data_vector.at(1);
  } else {
    auto navigation_data          = data_.getNavigationData();
    navigation_data.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setNavigationData(navigation_data);
    log_.error("At least two Encoders no longer reliable, entering CriticalFailure.");
  }
  return quartile_bounds;
}

void Navigation::imuOutlierDetection(NavigationArray &data_array)
{
  const QuartileBounds quartile_bounds = calculateImuQuartiles(data_array);
  static constexpr uint8_t threshold   = kInterQuartileScaler;

  // find the thresholds
  // clip IQR to upper bound to avoid issues with very large outliers
  const auto iqr = std::min(quartile_bounds.at(2) - quartile_bounds.at(0), kMaxInterQuartileRange);
  const auto upper_limit = quartile_bounds.at(2) + threshold * iqr;
  const auto lower_limit = quartile_bounds.at(0) - threshold * iqr;
  // replace any outliers with the median
  for (std::size_t i = 0; i < data::Sensors::kNumImus; ++i) {
    const bool exceeds_limits = data_array.at(i) < lower_limit || data_array.at(i) > upper_limit;
    if (exceeds_limits && is_imu_reliable_.at(i)) {
      log_.debug("Outlier detected in IMU %d, reading: %.3f not in [%.3f, %.3f]. Updated to %.3f",
                 i + 1, data_array.at(i), lower_limit, upper_limit, quartile_bounds.at(1));
      data_array.at(i) = quartile_bounds.at(1);
      imu_outlier_counter_.at(i)++;
      // If this counter exceeds some threshold then that IMU is deemed unreliable
      if (imu_outlier_counter_.at(i) > 1000 && is_imu_reliable_.at(i)) {
        is_imu_reliable_.at(i) = false;
        ++num_outlier_imus_;
      }
      if (num_outlier_imus_ > 1) {
        status_ = data::ModuleStatus::kCriticalFailure;
        log_.error("At least two IMUs no longer reliable, entering CriticalFailure.");
      }
    } else {
      imu_outlier_counter_.at(i) = 0;
    }
  }
}

void Navigation::wheelEncoderOutlierDetection(EncoderArray &data_array)
{
  const QuartileBounds quartile_bounds = calculateEncoderQuartiles(data_array);
  static constexpr uint8_t threshold   = kInterQuartileScaler;

  // find the thresholds
  // clip IQR to upper bound to avoid issues with very large outliers
  const auto iqr = std::min(quartile_bounds.at(2) - quartile_bounds.at(0), kMaxInterQuartileRange);
  const auto upper_limit = quartile_bounds.at(2) + threshold * iqr;
  const auto lower_limit = quartile_bounds.at(0) - threshold * iqr;
  // replace any outliers with the median
  for (std::size_t i = 0; i < data::Sensors::kNumEncoders; ++i) {
    const bool exceeds_limits = data_array.at(i) < lower_limit || data_array.at(i) > upper_limit;
    if (exceeds_limits && is_encoder_reliable_.at(i)) {
      log_.debug(
        "Outlier detected in Encoder %d, reading: %.3f not in [%.3f, %.3f]. Updated to %.3f", i + 1,
        data_array.at(i), lower_limit, upper_limit, quartile_bounds.at(1));
      data_array.at(i) = quartile_bounds.at(1);
      encoder_outlier_counter_.at(i)++;
      // If this counter exceeds some threshold then that encoder is deemed unreliable
      if (encoder_outlier_counter_.at(i) > 1000 && is_encoder_reliable_.at(i)) {
        is_encoder_reliable_.at(i) = false;
        ++num_outlier_encoders_;
      }
      if (num_outlier_encoders_ > 1) {
        status_ = data::ModuleStatus::kCriticalFailure;
        log_.error("At least two Wheel Encoders no longer reliable, entering CriticalFailure.");
      }
    } else {
      encoder_outlier_counter_.at(i) = 0;
    }
  }
}

void Navigation::updateData()
{
  data::Navigation nav_data;
  nav_data.module_status              = getModuleStatus();
  nav_data.displacement               = getImuDisplacement();
  nav_data.velocity                   = getImuVelocity();
  nav_data.acceleration               = getImuAcceleration();
  nav_data.emergency_braking_distance = getEmergencyBrakingDistance();
  nav_data.braking_distance           = 1.2 * getEmergencyBrakingDistance();

  data_.setNavigationData(nav_data);

  if (log_counter_ % 100 == 0) {
    log_.debug("%d: Data Update: a=%.3f, v=%.3f, d=%.3f", log_counter_, nav_data.acceleration,
               nav_data.velocity, nav_data.displacement);
    log_.debug("%d: Data Update: v(unc)=%.3f, d(unc)=%.3f", log_counter_, velocity_uncertainty_,
               displacement_uncertainty_);
  }
  ++log_counter_;
  // Update all prev measurements
  previous_timestamp_    = displacement_.timestamp;
  previous_acceleration_ = getImuAcceleration();
  previous_velocity_     = getImuVelocity();
}

void Navigation::navigate()
{
  queryImus();
  queryWheelEncoders();
  compareEncoderImu();
  if (log_counter_ > 1000) updateUncertainty();
  updateData();
}

void Navigation::initialiseTimestamps()
{
  // First iteration --> set timestamps
  const auto initial_timestamp = utils::Timer::getTimeMicros();
  acceleration_.timestamp      = initial_timestamp;
  velocity_.timestamp          = initial_timestamp;
  displacement_.timestamp      = initial_timestamp;
  previous_acceleration_       = getImuAcceleration();
  previous_velocity_           = getImuVelocity();
  initial_timestamp_           = initial_timestamp;
  log_.debug("Initial timestamp:%d", initial_timestamp_);
  previous_timestamp_ = initial_timestamp;
}
}  // namespace hyped::navigation