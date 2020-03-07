/*
 * Author: Neil McBlane, Brano Pilnan, Justus Rudolph
 * Organisation: HYPED
 * Date: 16/02/2020
 * Description: Main file for navigation class.
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#include <algorithm>

#include "navigation/navigation.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/timer.hpp"

namespace hyped {

using hyped::utils::concurrent::Thread;

namespace navigation {

Navigation::Navigation(Logger& log, unsigned int axis/*=0*/)
         : log_(log),
           data_(Data::getInstance()),
           status_(ModuleStatus::kStart),
           counter_(0),
           axis_(axis),
           calibration_limits_ {{0.05, 0.05, 0.05}},
           curr_msmt_(0),
           imu_reliable_ {{true, true, true, true}},
           nOutlierImus_(0),
           stripe_counter_(log_, data_, displ_unc_,
            velocity_uncertainty_, kStripeDistance),
           keyence_used_(true),
           keyence_real_(true),
           acceleration_(0, 0.),
           velocity_(0, 0.),
           displacement_(0, 0.),
           displ_unc_(0.),
           velocity_uncertainty_(0.),
           init_time_set_(false),
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

NavigationType Navigation::getAcceleration() const
{
  return acceleration_.value;
}

NavigationType Navigation::getVelocity() const
{
  return velocity_.value;
}

NavigationType Navigation::getDisplacement() const
{
  return displacement_.value;
}

NavigationType Navigation::getEmergencyBrakingDistance() const
{
  // TODO(Anyone): Account for actuation delay and/or communication latency?
  // Also, how realistic is this? (e.g brake force wearing down)
  return getVelocity()*getVelocity() / (2*kEmergencyDeceleration);
}

NavigationType Navigation::getBrakingDistance() const
{
  Motors motor_data = data_.getMotorData();
  uint32_t rpm = 0;
  for (int i = 0; i < data::Motors::kNumMotors; i++) {
    rpm += motor_data.rpms[i];
  }
  uint32_t avg_rpm = rpm / data::Motors::kNumMotors;
  float rot_velocity = (avg_rpm / 60) * (2 * pi);

  NavigationType actuation_force = spring_compression_ * spring_coefficient_;
  NavigationType braking_force = (actuation_force * coeff_friction_) /
                                 (tan(embrake_angle_) - coeff_friction_);
  NavigationType deceleration_total = kNumBrakes * braking_force / pod_mass_;

  NavigationType pod_kinetic_energy = 0.5 * pod_mass_ * getVelocity() * getVelocity();
  NavigationType rotational_kinetic_energy = data::Motors::kNumMotors * 0.5 * mom_inertia_wheel_ *
                                             rot_velocity * rot_velocity;
  NavigationType total_kinetic_energy = pod_kinetic_energy + rotational_kinetic_energy;

  NavigationType braking_distance = (total_kinetic_energy / pod_mass_) / deceleration_total;

  return braking_distance;
}

Navigation::NavigationVectorArray Navigation::getGravityCalibration() const
{
  return gravity_calibration_;
}

void Navigation::calibrateGravity()
{
  log_.INFO("NAV", "Calibrating gravity");
  array<RollingStatistics<NavigationVector>, Sensors::kNumImus> online_array =
    {{ RollingStatistics<NavigationVector>(kCalibrationQueries),
       RollingStatistics<NavigationVector>(kCalibrationQueries),
       RollingStatistics<NavigationVector>(kCalibrationQueries),
       RollingStatistics<NavigationVector>(kCalibrationQueries) }};
  bool calibration_successful = false;
  int calibration_attempts = 0;

  while (!calibration_successful && calibration_attempts < kCalibrationAttempts) {
    log_.INFO("NAV", "Calibration attempt %d", calibration_attempts+1);

    // Average each sensor over specified number of readings
    for (int i = 0; i < kCalibrationQueries; ++i) {
      sensor_readings_ = data_.getSensorsImuData();
      for (int j = 0; j < Sensors::kNumImus; ++j) {
        NavigationVector imu_data = sensor_readings_.value[j].acc;
        online_array[j].update(imu_data);
        if (nav_write_) {
          std::ofstream writefile;
          writefile.open("src/navigation/testing/nav_data.csv", std::ios::app);
          writefile << j           << delimiter << imu_data[0] << delimiter <<
                       imu_data[1] << delimiter << imu_data[2] << std::endl;
          writefile.close();
        }
      }
      Thread::sleep(1);
    }
    // Check if each calibration's variance is acceptable
    calibration_successful = true;
    for (int i = 0; i < Sensors::kNumImus; ++i) {
      for (int j = 0; j < 3; ++j) {
        bool var_within_lim = online_array[i].getVariance()[j] < calibration_limits_[j];
        calibration_successful = calibration_successful && var_within_lim;
      }
    }
    calibration_attempts++;
  }

  // Store calibration and update filters if successful
  if (calibration_successful) {
    log_.INFO("NAV", "Calibration of IMU acceleration succeeded with final readings:");
    for (int i = 0; i < Sensors::kNumImus; ++i) {
      gravity_calibration_[i] = online_array[i].getMean();
      double var = 0.0;
      for (int j = 0; j < 3; ++j) {
        var += online_array[i].getVariance()[j];
      }
      filters_[i].updateMeasurementCovarianceMatrix(var);

      log_.INFO("NAV", "\tIMU %d: g=(%.5f, %.5f, %.5f), var=%.5f",
              i, gravity_calibration_[i][0], gravity_calibration_[i][1],
              gravity_calibration_[i][2], var);
    }
    // set calibration uncertainties
    for (int axis = 0; axis < 3; axis++) {
      for (int i = 0; i < Sensors::kNumImus; i++) {
        double var = (online_array[i].getVariance()[axis]);
        calibration_variance_[axis] += var*var;
      }
      // geometric mean for variances of different IMUs
      calibration_variance_[axis] = sqrt(calibration_variance_[axis]);
    }
    log_.INFO("NAV", "Calibration Variance: x-axis: %.3f, y-axis: %.3f, z-axis: %.3f",
      calibration_variance_[0], calibration_variance_[1], calibration_variance_[2]);
    status_ = ModuleStatus::kReady;
    updateData();
    log_.INFO("NAV", "Navigation module ready");
  } else {
    log_.INFO("NAV", "Calibration of IMU acceleration failed with final readings:");
    for (int i = 0; i < Sensors::kNumImus; ++i) {
      NavigationVector acc = online_array[i].getMean();
      double var = 0.0;
      for (int j = 0; j < 3; ++j) {
        var += online_array[i].getVariance()[j];
      }

      log_.INFO("NAV", "\tIMU %d: g=(%.5f, %.5f, %.5f), var=%.5f", i, acc[0], acc[1], acc[2], var);
    }
    status_ = ModuleStatus::kCriticalFailure;
    updateData();
    log_.ERR("NAV", "Navigation module failed on calibration");
  }
}

NavigationType Navigation::accNorm(NavigationVector& acc)
{
  NavigationType norm = 0.0;
  for (unsigned int i = 0; i < 3; i ++) {
      NavigationType a = acc[i];
      norm += a*a;
  }
  norm = sqrt(norm);
  return norm;
}

void Navigation::queryImus()
{
  ImuAxisData acc_raw;  // All raw data, four values per axis
  NavigationArray acc_raw_moving;  // Raw values in moving axis

  OnlineStatistics<NavigationType> acc_avg_filter;
  sensor_readings_ = data_.getSensorsImuData();
  uint32_t t = sensor_readings_.timestamp;
  // process raw values
  for (uint8_t axis = 0; axis < 3; axis++) {
    for (int i = 0; i < Sensors::kNumImus; ++i) {
      NavigationVector a = sensor_readings_.value[i].acc - gravity_calibration_[i];
      acc_raw[axis][i] = a[axis];

      // the moving axis should be set to 0 for tukeyFences
      if (!imu_reliable_[i]) {
        acc_raw_moving[i] = 0;
      } else if (axis == axis_) {
        acc_raw_moving[i] = a[axis_];
      }
    }
  }
  log_.DBG1("NAV", "Raw acceleration values: %.3f, %.3f, %.3f, %.3f", acc_raw_moving[0],
            acc_raw_moving[1], acc_raw_moving[2], acc_raw_moving[3]);
  // Run outlier detection on moving axis
  tukeyFences(acc_raw_moving, kTukeyThreshold);
  // TODO(Justus) how to run outlier detection on non-moving axes without affecting "reliable"
  // Current idea: outlier function takes reliability write flag, on hold until z-score impl.

  /*for (int axis = 0; axis < 3; axis++) {
    if (axis != axis_) tukeyFences(acc_raw[axis], kTukeyThreshold);
  }*/

  // Kalman filter the readings which are reliable
  for (int i = 0; i < Sensors::kNumImus; ++i) {
    if (imu_reliable_[i]) {
      NavigationType estimate = filters_[i].filter(acc_raw_moving[i]);
      acc_avg_filter.update(estimate);
    }
  }
  previous_measurements_[curr_msmt_] = acc_raw;
  curr_msmt_++;
  if (curr_msmt_ == kPreviousMeasurements) {
    curr_msmt_ = 0;
    prev_filled_ = 1;
  }
  if (prev_filled_) checkVibration();

  acceleration_.value = acc_avg_filter.getMean();
  acceleration_.timestamp = t;

  acceleration_integrator_.update(acceleration_);
  velocity_integrator_.update(velocity_);
}

void Navigation::checkVibration()
{
  // curr_msmt points at next measurement, ie the last one
  array<OnlineStatistics<NavigationType>, 3> online_array_axis;
  for (int i = 0; i < kPreviousMeasurements; i++) {
    ImuAxisData raw_data = previous_measurements_[(curr_msmt_ + i) % kPreviousMeasurements];
    for (uint8_t axis = 0; axis < 3; axis++) {
      if (axis != axis_) {  // assume variance in moving axis are not vibrations
        for (int imu = 0; imu < Sensors::kNumImus; imu++) {
          online_array_axis[axis].update(raw_data[axis][imu]);
        }
      }
    }
  }
  for (uint8_t axis = 0; axis < 3; axis++) {
    double var = online_array_axis[axis].getVariance();
    if (counter_ % 100000 == 0 && axis != axis_) {
      log_.INFO("NAV", "Variance in axis %d: %.3f", axis, var);
    }
    double ratio = var / calibration_variance_[axis];
    double statistical_variance_ratio = kCalibrationQueries/kPreviousMeasurements;
    if (ratio > statistical_variance_ratio) {
      log_.ERR("NAV", "Variance in axis %d is %.3f times larger than its calibration variance.",
        axis, ratio);
    }
  }
}

void Navigation::updateUncertainty()
{
  double delta_t = (displacement_.timestamp - prev_timestamp_)/1000000.0;
  NavigationType abs_delta_acc = abs(getAcceleration() - prev_acc_);
  // Random walk uncertainty
  velocity_uncertainty_ += abs_delta_acc*delta_t/2.;
  // Processing uncertainty
  NavigationType acc_variance = 0.0;
  for (int i = 0; i < Sensors::kNumImus; i++) {
    acc_variance += filters_[i].KalmanFilter::getEstimateVariance();
  }
  acc_variance = acc_variance/Sensors::kNumImus;
  NavigationType acc_stdDev = sqrt(acc_variance);

  velocity_uncertainty_ += acc_stdDev*delta_t;
  displ_unc_ += velocity_uncertainty_*delta_t;
  // Random walk uncertainty
  displ_unc_ += abs(getVelocity() - prev_vel_) * delta_t / 2.;
}

void Navigation::disableKeyenceUsage()
{
  keyence_used_ = false;
}

void Navigation::setKeyenceFake()
{
  keyence_real_ = false;
}

bool Navigation::getHasInit()
{
  return init_time_set_;
}

void Navigation::setHasInit()
{
  init_time_set_ = true;
}

void Navigation::logWrite()
{
  nav_write_ = true;
}

void Navigation::tukeyFences(NavigationArray& data_array, float threshold)
{
  // Define the quartiles first:
  float q1 = 0;
  float q2 = 0;
  float q3 = 0;
  // The most likely case is that all four IMUs are still reliable:
  if (nOutlierImus_ == 0) {
    // copy the original array for sorting
    NavigationArray data_array_copy = data_array;
    // find the quartiles
    std::sort(data_array_copy.begin(), data_array_copy.end());
    q1 = (data_array_copy[0]+data_array_copy[1]) / 2.;
    q2 = (data_array_copy[1]+data_array_copy[2]) / 2.;
    q3 = (data_array_copy[2]+data_array_copy[3]) / 2.;
  // The second case is that one IMU is faulty
  } else if (nOutlierImus_ == 1) {
    // select non-outlier values
    NavigationArrayOneFaulty data_array_faulty;
    if (!imu_reliable_[0]) {
      data_array_faulty = {{data_array[1], data_array[2], data_array[3]}};
    } else if (!imu_reliable_[1]) {
      data_array_faulty = {{data_array[0], data_array[2], data_array[3]}};
    } else if (!imu_reliable_[2]) {
      data_array_faulty = {{data_array[0], data_array[1], data_array[3]}};
    } else if (!imu_reliable_[3]) {
      data_array_faulty = {{data_array[0], data_array[1], data_array[2]}};
    }
    std::sort(data_array_faulty.begin(), data_array_faulty.end());
    q1 = (data_array_faulty[0] + data_array_faulty[1]) / 2.;
    q2 =  data_array_faulty[1];
    q3 = (data_array_faulty[1] + data_array_faulty[2]) / 2.;
  } else if (nOutlierImus_ < 4) {
    // set all 0.0 IMUs to non-zero avg
    float sum_non_outliers = 0.0;
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
  float iqr = q3 - q1;
  // clip IQR to upper bound to avoid issues with very large outliers
  if (iqr > kTukeyIQRBound) {
    iqr = kTukeyIQRBound;
  }
  float upper_limit = q3 + threshold*iqr;
  float lower_limit = q1 - threshold*iqr;
  // replace any outliers with the median
  for (int i = 0; i < Sensors::kNumImus; ++i) {
    if ((data_array[i] < lower_limit or data_array[i] > upper_limit) && imu_reliable_[i]) {
      log_.DBG3("NAV", "Outlier detected in IMU %d, reading: %.3f not in [%.3f, %.3f]. Updated to %.3f", //NOLINT
                i+1, data_array[i], lower_limit, upper_limit, q2);
      // log_.DBG3("NAV", "Outlier detected with quantiles: %.3f, %.3f, %.3f", q1, q2, q3);

      data_array[i] = q2;
      imu_outlier_counter_[i]++;
      // If this counter exceeds some threshold then that IMU is deemed unreliable
      if (imu_outlier_counter_[i] > 1000 && imu_reliable_[i]) {
        // log_.DBG3("NAV", "IMU%d is an outlier!", i + 1);
        imu_reliable_[i] = false;
        nOutlierImus_++;
      }
      if (nOutlierImus_ > 1) {
        status_ = ModuleStatus::kCriticalFailure;
        log_.ERR("NAV", "At least two IMUs no longer reliable, entering CriticalFailure.");
      }
    } else {
      imu_outlier_counter_[i] = 0;
      if (counter_ % 100 == 0 && imu_reliable_[i]) {
        /*
         * log_.DBG3("NAV", "No Outlier detected in IMU %d, reading: %.3f in [%.3f, %.3f]",
         *           i+1, data_array[i], lower_limit, upper_limit);
         */
      }
    }
  }
  /*
   * if (counter_ % 100 == 0) {
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

  if (counter_ % 100 == 0) {  // kPrintFreq
    log_.DBG("NAV", "%d: Data Update: a=%.3f, v=%.3f, d=%.3f, d(keyence)=%.3f", //NOLINT
               counter_, nav_data.acceleration, nav_data.velocity, nav_data.displacement,
               stripe_counter_.getStripeCount()*kStripeDistance);
    log_.DBG("NAV", "%d: Data Update: v(unc)=%.3f, d(unc)=%.3f, keyence failures: %d",
               counter_, velocity_uncertainty_, displ_unc_, stripe_counter_.getFailureCount());
  }
  counter_++;
  // Update all prev measurements
  prev_timestamp_ = displacement_.timestamp;
  prev_acc_ = getAcceleration();
  prev_vel_ = getVelocity();
}

void Navigation::navigate()
{
  queryImus();
  if (keyence_used_) {
    stripe_counter_.queryKeyence(displacement_.value, velocity_.value, keyence_real_);
    if (stripe_counter_.checkFailure(displacement_.value))
      status_ = ModuleStatus::kCriticalFailure;
  }
  if (counter_ > 1000) updateUncertainty();
  updateData();
}

void Navigation::initTimestamps()
{
  // First iteration --> set timestamps
  acceleration_.timestamp = utils::Timer::getTimeMicros();
  velocity_    .timestamp = utils::Timer::getTimeMicros();
  displacement_    .timestamp = utils::Timer::getTimeMicros();
  prev_acc_ = getAcceleration();
  prev_vel_ = getVelocity();
  init_timestamp_ = utils::Timer::getTimeMicros();
  log_.DBG3("NAV", "Initial timestamp:%d", init_timestamp_);
  prev_timestamp_ = utils::Timer::getTimeMicros();
  stripe_counter_.set_init(init_timestamp_);
}
}}  // namespace hyped::navigation
