#pragma once

#include "kalman_filter.hpp"

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
  using QuartileBounds           = std::array<data::nav_t, 3>;
  using EncoderArray             = std::array<uint32_t, data::Sensors::kNumEncoders>;

  /**
   * @brief Construct a new Navigation object
   *
   * @param log System logger
   * @param axis Axis used of acceleration measurements
   */
  explicit Navigation(const std::uint32_t axis = 0);
  /**
   * @brief Get the current wheel encoder displacement
   *
   * @return encoder_displacement_ the current wheel encoder displacement
   */
  data::nav_t getEncoderDisplacement() const;
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
  data::nav_t getImuAcceleration() const;
  /**
   * @brief Get the measured velocity [m/s]
   *
   * @return nav_t Returns the forward component of velocity vector [m/s]
   */
  data::nav_t getImuVelocity() const;
  /**
   * @brief Get the measured displacement [m]
   *
   * @return nav_t Returns the forward component of displacement vector [m]
   */
  data::nav_t getImuDisplacement() const;
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
   * @brief Calculate quartiles for an array of readings. Updates quartile_bounds array
   *
   * @param pointer to array of original acceleration readings
   *
   * @return quartiles of reliable IMU readings of form (q1, q2(median), q3)
   */
  QuartileBounds calculateImuQuartiles(const NavigationArray &data_array);
  /**
   * @brief Calculate quartiles for an array of readings. Updates quartile_bounds array
   *
   * @param pointer to array of original acceleration readings
   *
   * @return quartiles of reliable IMU readings of form (q1, q2(median), q3)
   */
  QuartileBounds calculateEncoderQuartiles(const EncoderArray &data_array);
  /**
   * @brief Apply scaled interquartile range bounds on an array of readings
   *
   * @param pointer to array of original acceleration readings
   */
  void imuOutlierDetection(NavigationArray &data_array);
  /**
   * @brief Apply scaled interquartile range bounds on an array of readings
   *
   * @param pointer to array of original encoder readings
   */
  void wheelEncoderOutlierDetection(EncoderArray &data_array);
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
  static constexpr data::nav_t kInterQuartileScaler   = 1;
  static constexpr data::nav_t kMaxInterQuartileRange = 3;

  static constexpr data::nav_t kPodMass              = 250;   // kg
  static constexpr data::nav_t kMomentOfInertiaWheel = 0.04;  // kgm??
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

  // Counter for consecutive outlier output from each wheel encoder
  std::array<uint32_t, data::Sensors::kNumEncoders> encoder_outlier_counter_;
  // Array of booleans to signify which encoders are reliable or faulty
  std::array<bool, data::Sensors::kNumEncoders> is_encoder_reliable_;
  // Counter of how many encoders have failed
  uint32_t num_outlier_encoders_;

  // To store estimated values
  ImuDataPointArray sensor_readings_;
  data::DataPoint<data::nav_t> encoder_displacement_;
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

  // To convert acceleration -> velocity -> distance
  utils::math::Integrator<data::nav_t> acceleration_integrator_;  // acceleration to velocity
  utils::math::Integrator<data::nav_t> velocity_integrator_;      // velocity to distance
  /**
   * @brief Query sensors to determine velocity and distance
   */
  void queryWheelEncoders();
  /**
   * @brief Query sensors to determine acceleration, velocity and distance
   */
  void queryImus();
  /**
   * @brief Update uncertainty in distance obtained through IMU measurements.
   */
  void updateUncertainty();
  /**
   * @brief Check for vibrations
   */
  void checkVibration();
  /**
   * @brief Compare keyence estimate and imu estimate for velocity
   */
  void compareEncoderImu();
};

}  // namespace hyped::navigation
