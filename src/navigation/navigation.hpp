#pragma once

#include <math.h>

#include <array>
#include <cstdint>
#include <data/data.hpp>
#include <data/data_point.hpp>
#include <fstream>
#include <sensors/imu.hpp>
#include <utils/logger.hpp>
#include <utils/math/integrator.hpp>
#include <utils/math/statistics.hpp>

#include "kalman_filter.hpp"
#include "stripe_handler.hpp"

namespace hyped {

using data::Data;
using data::DataPoint;
using data::ImuData;
using data::ModuleStatus;
using data::Motors;
using data::nav_t;
using data::NavigationVector;
using data::Sensors;
using navigation::KalmanFilter;
using std::array;
using utils::Logger;
using utils::math::Integrator;
using utils::math::OnlineStatistics;
using utils::math::RollingStatistics;

namespace navigation {

class Navigation {
 public:
  typedef array<ImuData, Sensors::kNumImus> ImuDataArray;
  typedef DataPoint<ImuDataArray> ImuDataPointArray;
  typedef array<NavigationVector, Sensors::kNumImus> NavigationVectorArray;
  typedef array<array<nav_t, Sensors::kNumImus>, 3> ImuAxisData;
  typedef array<nav_t, Sensors::kNumImus> NavigationArray;
  typedef array<nav_t, Sensors::kNumImus - 1> NavigationArrayOneFaulty;
  typedef array<KalmanFilter, Sensors::kNumImus> FilterArray;

  /**
   * @brief Construct a new Navigation object
   *
   * @param log System logger
   * @param axis Axis used of acceleration measurements
   */
  explicit Navigation(Logger &log, unsigned int axis = 0);
  /**
   * @brief Get the current state of the navigation module
   *
   * @return ModuleStatus the current state of the navigation module
   */
  ModuleStatus getModuleStatus() const;
  /**
   * @brief Get the measured acceleration [m/s^2]
   *
   * @return nav_t Returns the forward component of acceleration vector (negative when
   *                        decelerating) [m/s^2]
   */
  nav_t getAcceleration() const;
  /**
   * @brief Get the measured velocity [m/s]
   *
   * @return nav_t Returns the forward component of velocity vector [m/s]
   */
  nav_t getVelocity() const;
  /**
   * @brief Get the measured displacement [m]
   *
   * @return nav_t Returns the forward component of displacement vector [m]
   */
  nav_t getDisplacement() const;
  /**
   * @brief Get the emergency braking distance [m]
   *
   * @return nav_t emergency braking distance [m]
   */
  nav_t getEmergencyBrakingDistance() const;
  /**
   * @brief Get the braking distance [m]
   *
   * @return nav_t braking distance [m]
   */
  nav_t getBrakingDistance() const;
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
  void tukeyFences(NavigationArray &data_array, float threshold);
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
  /**
   * @brief Enable writing to file nav_data.csv
   */
  void logWrite();

 private:
  static constexpr int kCalibrationAttempts = 3;
  static constexpr int kCalibrationQueries  = 10000;

  // number of previous measurements stored
  static constexpr int kPreviousMeasurements = 1000;

  static constexpr char kDelimiter = '\t';

  static constexpr int kPrintFreq               = 1;
  static constexpr nav_t kEmergencyDeceleration = 24;
  static constexpr float kTukeyThreshold        = 1;  // 0.75
  static constexpr float kTukeyIQRBound         = 3;

  static constexpr nav_t kStripeDistance = 30.48;

  static constexpr uint32_t pod_mass_           = 250;   // kg
  static constexpr float mom_inertia_wheel_     = 0.04;  // kgm²
  static constexpr uint32_t kNumBrakes          = 4;
  static constexpr float coeff_friction_        = 0.38;
  static constexpr uint32_t spring_compression_ = 40;
  static constexpr uint32_t spring_coefficient_ = 18;
  static constexpr float embrake_angle_         = 0.52;

  static constexpr float pi = 3.14159265359;  // Have to approximate

  // System communication
  Logger &log_;
  Data &data_;
  ModuleStatus status_;

  // counter for outputs
  unsigned int counter_;

  // movement axis
  unsigned int axis_;

  // acceptable variances for calibration measurements: {x, y, z}
  array<float, 3> calibration_limits_;
  // Calibration variances in each dimension, necessary for vibration checking
  array<nav_t, 3> calibration_variance_;

  // Array of previous measurements
  array<ImuAxisData, kPreviousMeasurements> previous_measurements_;
  // Current point in recent measurements, to save space
  uint16_t curr_msmt_;
  // Boolean value to check if the array has been filled, to not wrong variance
  bool prev_filled_;

  // Flag to write to file
  bool nav_write_;

  // Kalman filters to filter each IMU measurement individually
  FilterArray filters_;

  // Counter for consecutive outlier output from each IMU
  array<uint32_t, Sensors::kNumImus> imu_outlier_counter_;
  // Array of booleans to signify which IMUs are reliable or faulty
  array<bool, Sensors::kNumImus> imu_reliable_;
  // Counter of how many IMUs have failed
  uint32_t nOutlierImus_;

  // To store estimated values
  ImuDataPointArray sensor_readings_;
  DataPoint<nav_t> acceleration_;
  DataPoint<nav_t> velocity_;
  DataPoint<nav_t> displacement_;
  NavigationVectorArray gravity_calibration_;

  // Initial timestamp (for comparisons)
  uint32_t init_timestamp_;
  // Previous timestamp
  uint32_t prev_timestamp_;
  // Uncertainty in distance
  nav_t displ_unc_;
  // Uncertainty in velocity
  nav_t vel_unc_;
  // Previous acceleration measurement, necessary for uncertainty determination
  nav_t prev_acc_;
  // Previous velocity measurement
  nav_t prev_vel_;
  // Have initial timestamps been set?
  bool init_time_set_;

  // Stripe counter object
  StripeHandler stripe_counter_;
  // Flags if keyences are used and if real
  bool keyence_used_;
  bool keyence_real_;

  // To convert acceleration -> velocity -> distance
  Integrator<nav_t> acceleration_integrator_;  // acceleration to velocity
  Integrator<nav_t> velocity_integrator_;      // velocity to distance

  /**
   * @brief Compute norm of acceleration measurement
   */
  nav_t accNorm(NavigationVector &acc);
  /**
   * @brief Query sensors to determine acceleration, velocity and distance
   */
  void queryImus();
  /**
   * @brief Query Keyence sensors to determine whether a stripe is found, update stripe_counter_
   * accordingly
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

}  // namespace navigation
}  // namespace hyped
