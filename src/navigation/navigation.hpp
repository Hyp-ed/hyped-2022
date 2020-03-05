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

#ifndef NAVIGATION_NAVIGATION_HPP_
#define NAVIGATION_NAVIGATION_HPP_

#include <array>
#include <cstdint>
#include <math.h>

#include "data/data.hpp"
#include "data/data_point.hpp"
#include "sensors/imu.hpp"
#include "navigation/kalman_filter.hpp"
#include "utils/logger.hpp"
#include "utils/math/integrator.hpp"
#include "utils/math/statistics.hpp"

namespace hyped {

using data::Data;
using data::DataPoint;
using data::ImuData;
using data::ModuleStatus;
using data::NavigationType;
using data::NavigationVector;
using data::Motors;
using data::Sensors;
using navigation::KalmanFilter;
using utils::Logger;
using utils::math::Integrator;
using utils::math::OnlineStatistics;
using utils::math::RollingStatistics;
using std::array;

namespace navigation {

  class Navigation {
    public:
      typedef array<ImuData, Sensors::kNumImus>                   ImuDataArray;
      typedef DataPoint<ImuDataArray>                             ImuDataPointArray;
      typedef array<NavigationVector, Sensors::kNumImus>          NavigationVectorArray;
      typedef array<array<NavigationType, Sensors::kNumImus>, 3>  ImuAxisData;
      typedef array<NavigationType, Sensors::kNumImus>            NavigationArray;
      typedef array<NavigationType, Sensors::kNumImus-1>          NavigationArrayOneFaulty;
      typedef array<KalmanFilter, Sensors::kNumImus>              FilterArray;
      typedef array<data::StripeCounter, Sensors::kNumKeyence>    KeyenceDataArray;

      /**
       * @brief Construct a new Navigation object
       *
       * @param log System logger
       * @param axis Axis used of acceleration measurements
       */
      explicit Navigation(Logger& log, unsigned int axis = 0);
      /**
       * @brief Get the current state of the navigation module
       *
       * @return ModuleStatus the current state of the navigation module
       */
      ModuleStatus getModuleStatus() const;
      /**
       * @brief Get the measured acceleration [m/s^2]
       *
       * @return NavigationType Returns the forward component of acceleration vector (negative when
       *                        decelerating) [m/s^2]
       */
      NavigationType getAcceleration() const;
      /**
       * @brief Get the measured velocity [m/s]
       *
       * @return NavigationType Returns the forward component of velocity vector [m/s]
       */
      NavigationType getVelocity() const;
      /**
       * @brief Get the measured displacement [m]
       *
       * @return NavigationType Returns the forward component of displacement vector [m]
       */
      NavigationType getDistance() const;
      /**
       * @brief Get the emergency braking distance [m]
       *
       * @return NavigationType emergency braking distance [m]
       */
      NavigationType getEmergencyBrakingDistance() const;
      /**
       * @brief Get the braking distance [m]
       *
       * @return NavigationType braking distance [m]
       */
      NavigationType getBrakingDistance() const;
      /**
       * @brief Get the determined gravity calibration [m/s^2]
       *
       * @return NavitationArray recorded gravitational acceleration [m/s^2]
       */
      NavigationVectorArray getGravityCalibration() const;
      /**
       * @brief Determine the value of gravitational acceleration measured by sensors at rest
       */
      void calibrateGravity();
      /**
       * @brief Apply Tukey's fences to an array of readings
       *
       * @param pointer to array of original acceleration readings
       * @param threshold value
       */
      void tukeyFences(NavigationArray& data_array, float threshold);
      /**
       * @brief Update central data structure
       */
      void updateData();
      /**
       * @brief Take acceleration readings from IMU, filter, integrate and then update central data
       * structure with new values (i.e. the meat'n'potatoes of navigation).
       */
      void navigate();
      /**
       * @brief Initialise timestamps for integration
       */
      void initTimestamps();
      /**
       * @brief Used to check whether initial timestamps have been set
       *
       * @return Boolean whether init timestamps have been set
       */
      bool getHasInit();
      /*
       * @brief Set initialisation of timestamps to true
       */
      void setHasInit();
      /**
       * @brief Disable keyence readings to have any impact on the run.
       */
      void disableKeyenceUsage();
      /**
       * @brief Set the keyence used to fake, so the system knows to use central timestamps.
       */
      void setKeyenceFake();

    private:
      static constexpr int kCalibrationAttempts = 3;
      static constexpr int kCalibrationQueries = 10000;

      // number of previous measurements stored
      static constexpr int kPreviousMeasurements = 1000;

      static constexpr int kPrintFreq = 1;
      static constexpr NavigationType kEmergencyDeceleration = 24;
      static constexpr float kTukeyThreshold = 1;  // 0.75
      static constexpr float kTukeyIQRBound = 3;

      static constexpr NavigationType kStripeDistance = 30.48;

      static constexpr uint32_t pod_mass_           = 250;  // kg
      static constexpr float    mom_inertia_wheel_  = 0.04;  // kgm²
      static constexpr uint32_t kNumBrakes          = 4;
      static constexpr float    coeff_friction_     = 0.38;
      static constexpr uint32_t spring_compression_ = 40;
      static constexpr uint32_t spring_coefficient_ = 18;
      static constexpr float    embrake_angle_      = 0.52;

     static constexpr float pi = 3.14159265359;  // Have to approximate

      // System communication
      Logger& log_;
      Data& data_;
      ModuleStatus status_;

      // counter for outputs
      unsigned int counter_;

      // movement axis
      unsigned int axis_;

      // acceptable variances for calibration measurements: {x, y, z}
      array<float, 3> calibration_limits_;

      // Calibration variances in each dimension, necessary for vibration checking
      array<NavigationType, 3> calibration_variance_;

      // Array of previous measurements
      array<ImuAxisData, kPreviousMeasurements> previous_measurements_;
      // Current point in recent measurements, to save space
      uint16_t curr_msmt_;
      // Boolean value to check if the array has been filled, to not wrong variance
      bool prev_filled_;

      // Kalman filters to filter each IMU measurement individually
      FilterArray filters_;

      // Counter for consecutive outlier output from each IMU
      array<uint32_t, Sensors::kNumImus> imu_outlier_counter_;
      // Array of booleans to signify which IMUs are reliable or faulty
      array<bool, Sensors::kNumImus> imu_reliable_;
      // Counter of how many IMUs have failed
      uint32_t nOutlierImus_;

      // Stripe counter (rolling values)
      DataPoint<uint32_t> stripe_counter_;
      // Keyence data read
      KeyenceDataArray keyence_readings_;
      // Previous keyence data for comparison
      KeyenceDataArray prev_keyence_readings_;
      // Are the keyence sensors used or ignored?
      bool keyence_used_;
      // Is the keyence used fake or real?
      bool keyence_real_;
      // This counts the number of times the keyence readings disagree with the IMU data more than
      // allowed due to uncertainty. It is used at the moment to check if the calculated
      // uncertainty is acceptable.
      uint32_t keyence_failure_counter_;


      // To store estimated values
      ImuDataPointArray sensor_readings_;
      DataPoint<NavigationType> acceleration_;
      DataPoint<NavigationType> velocity_;
      DataPoint<NavigationType> distance_;
      NavigationVectorArray gravity_calibration_;

      // Initial timestamp (for comparisons)
      uint32_t init_timestamp_;
      // Previous timestamp
      uint32_t prev_timestamp_;
      // Uncertainty in distance
      NavigationType distance_uncertainty_;
      // Uncertainty in velocity
      NavigationType velocity_uncertainty_;
      // Previous acceleration measurement, necessary for uncertainty determination
      NavigationType prev_acc_;
      // Previous velocity measurement
      NavigationType prev_vel_;
      // Have initial timestamps been set?
      bool init_time_set_;

      // To convert acceleration -> velocity -> distance
      Integrator<NavigationType> acceleration_integrator_;  // acceleration to velocity
      Integrator<NavigationType> velocity_integrator_;      // velocity to distance

      /**
       * @brief Compute norm of acceleration measurement
       */
      NavigationType accNorm(NavigationVector& acc);
      /**
       * @brief Query sensors to determine acceleration, velocity and distance
       */
      void queryImus();
      /**
       * @brief Query Keyence sensors to determine whether a stripe is found, update stripe_counter_ accordingly
       */
      void queryKeyence();
      /**
       * @brief Update uncertainty in distance obtained through IMU measurements.
       */
      void updateUncertainty();
      /**
       * @brief Check for vibrations
       */
      void checkVibration();
  };


}}  // namespace hyped::navigation

#endif  // NAVIGATION_NAVIGATION_HPP_
