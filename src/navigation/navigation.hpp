#pragma once

#include "kalman_filter.hpp"
#include "stripe_handler.hpp"

#include <math.h>

#include <array>
#include <cstdint>
#include <fstream>

#include <data/data.hpp>
#include <data/data_point.hpp>
#include <sensors/imu.hpp>
#include <utils/logger.hpp>
#include <utils/math/integrator.hpp>
#include <utils/math/statistics.hpp>

namespace hyped::navigation {

class Navigation {
 public:
  using ImuDataArray             = std::array<data::ImuData, data::Sensors::kNumImus>;
  using ImuDataPointArray        = data::DataPoint<ImuDataArray>;
  using NavigationVectorArray    = std::array<data::NavigationVector, data::Sensors::kNumImus>;
  using ImuAxisData              = std::array<std::array<data::nav_t, data::Sensors::kNumImus>, 3>;
  using NavigationArray          = std::array<data::nav_t, data::Sensors::kNumImus>;
  using NavigationArrayOneFaulty = std::array<data::nav_t, data::Sensors::kNumImus - 1>;
  using FilterArray              = std::array<KalmanFilter, data::Sensors::kNumImus>;

  /**
   * @brief Construct a new Navigation object
   *
   * @param log System logger
   * @param axis Axis used of acceleration measurements
   */
  explicit Navigation(const std::uint32_t axis = 0);
  /**
   * @brief Get the current state of the navigation module
   *
   * @return ModuleStatus the current state of the navigation module
   */
  data::ModuleStatus getModuleStatus() const;
  /**
   * @brief Get the measured acceleration [m/s^2]
   *
   * @return nav_t Returns the forward component of acceleration vector (negative when
   *                        decelerating) [m/s^2]
   */
  data::nav_t getAcceleration() const;
  /**
   * @brief Get the measured velocity [m/s]
   *
   * @return nav_t Returns the forward component of velocity vector [m/s]
   */
  data::nav_t getVelocity() const;
  /**
   * @brief Get the measured displacement [m]
   *
   * @return nav_t Returns the forward component of displacement vector [m]
   */
  data::nav_t getDisplacement() const;
  /**
   * @brief Get the emergency braking distance [m]
   *
   * @return nav_t emergency braking distance [m]
   */
  data::nav_t getEmergencyBrakingDistance() const;
  /**
   * @brief Get the braking distance [m]
   *
   * @return nav_t braking distance [m]
   */
  data::nav_t getBrakingDistance() const;
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
  void tukeyFences(NavigationArray &data_array, const data::nav_t threshold);
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
  void initialiseTimestamps();
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
  static constexpr int kMaxCalibrationAttempts = 3;
  static constexpr int kCalibrationQueries     = 10000;

  // number of previous measurements stored
  static constexpr int kPreviousMeasurements = 1000;

  static constexpr char kDelimiter = '\t';

  static constexpr int kPrintFreq                     = 1;
  static constexpr data::nav_t kEmergencyDeceleration = 24;
  static constexpr data::nav_t kTukeyThreshold        = 1;  // 0.75
  static constexpr data::nav_t kTukeyIQRBound         = 3;

  static constexpr data::nav_t kPodMass              = 250;   // kg
  static constexpr data::nav_t kMomentOfInertiaWheel = 0.04;  // kgm²
  static constexpr uint32_t kNumBrakes               = 4;
  static constexpr data::nav_t kFrictionCoefficient  = 0.38;
  static constexpr data::nav_t kSpringCompression    = 40;
  static constexpr data::nav_t kSpringCoefficient    = 18;
  static constexpr data::nav_t kEmbrakeAngle         = 0.52;

  // System communication
  data::Data &data_;
  utils::Logger log_;
  data::ModuleStatus status_;

  uint32_t log_counter_;
  uint32_t movement_axis_;

  // acceptable variances for calibration measurements: {x, y, z}
  std::array<data::nav_t, 3> calibration_limits_;
  // Calibration variances in each dimension, necessary for vibration checking
  std::array<data::nav_t, data::Sensors::kNumImus> calibration_variance_;

  // Array of previous measurements
  std::array<ImuAxisData, kPreviousMeasurements> previous_measurements_;
  // Current point in recent measurements, to save space
  uint16_t current_measurements_;
  // Boolean value to check if the array has been filled, to not wrong variance
  bool previous_filled_;

  // Flag to write to file
  bool write_to_file_;

  // Kalman filters to filter each IMU measurement individually
  FilterArray filters_;

  // Counter for consecutive outlier output from each IMU
  std::array<uint32_t, data::Sensors::kNumImus> imu_outlier_counter_;
  // Array of booleans to signify which IMUs are reliable or faulty
  std::array<bool, data::Sensors::kNumImus> is_imu_reliable_;
  // Counter of how many IMUs have failed
  uint32_t num_outlier_imus_;

  // To store estimated values
  ImuDataPointArray sensor_readings_;
  data::DataPoint<data::nav_t> acceleration_;
  data::DataPoint<data::nav_t> velocity_;
  data::DataPoint<data::nav_t> displacement_;
  NavigationVectorArray gravity_calibration_;

  // Initial timestamp (for comparisons)
  uint32_t initial_timestamp_;
  // Previous timestamp
  uint32_t previous_timestamp_;
  // Uncertainty in distance
  data::nav_t displacement_uncertainty_;
  // Uncertainty in velocity
  data::nav_t velocity_uncertainty_;
  // Previous acceleration measurement, necessary for uncertainty determination
  data::nav_t previous_acceleration_;
  // Previous velocity measurement
  data::nav_t previous_velocity_;
  // Have initial timestamps been set?
  bool has_initial_time_;

  // Stripe counter object
  StripeHandler stripe_counter_;
  // Flags if keyences are used and if real
  bool is_keyence_used_;

  // To convert acceleration -> velocity -> distance
  utils::math::Integrator<data::nav_t> acceleration_integrator_;  // acceleration to velocity
  utils::math::Integrator<data::nav_t> velocity_integrator_;      // velocity to distance

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

}  // namespace hyped::navigation
