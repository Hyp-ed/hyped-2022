#pragma once

#include "data_point.hpp"

#include <array>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

#include <utils/concurrent/lock.hpp>
#include <utils/math/vector.hpp>

namespace hyped::data {

// -------------------------------------------------------------------------------------------------
// Global Module States
// -------------------------------------------------------------------------------------------------
enum class ModuleStatus {
  kCriticalFailure,  // state machine transitions to EmergencyBraking/FailureStopped
  kStart,            // Initial module state
  kInit,   // state machine transistions to Calibrating if all modules have Init or Ready status.
  kReady,  // state machine transistions to Ready if all modules have Ready status.
};

struct Module {
  ModuleStatus module_status = ModuleStatus::kStart;
};

// -------------------------------------------------------------------------------------------------
// Navigation
// -------------------------------------------------------------------------------------------------
using nav_t            = float;
using NavigationVector = utils::math::Vector<nav_t, 3>;
struct Navigation : public Module {
  static constexpr nav_t kMaximumVelocity   = 100;     // m/s
  static constexpr nav_t kRunLength         = 1250.0;  // m
  static constexpr nav_t kBrakingBuffer     = 20.0;    // m
  static constexpr nav_t kPi                = 3.14159265359;
  static constexpr nav_t kWheelCircumfrence = kPi * 0.1;  // m
  static constexpr nav_t kStripeDistance    = 15;         // m
  nav_t displacement;                                     // m
  nav_t velocity;                                         // m/s
  nav_t acceleration;                                     // m/s^2
  nav_t emergency_braking_distance;                       // m
  nav_t braking_distance = 750;                           // m
};

// -------------------------------------------------------------------------------------------------
// Raw Sensor data
// -------------------------------------------------------------------------------------------------
struct SensorData {
  bool operational;
};

struct ImuData : public SensorData {
  NavigationVector acc;
  std::vector<NavigationVector> fifo;
};

struct CounterData : public DataPoint<uint32_t>, public SensorData {
};

struct TemperatureData : public SensorData {
  int16_t temperature;  // C
};

struct AmbientPressureData : public SensorData {
  uint16_t ambient_pressure;  // mbar
};

struct BrakePressureData : public SensorData {
  uint16_t brake_pressure;  // bar
};

struct Sensors : public Module {
  static constexpr size_t kNumImus          = 4;
  static constexpr size_t kNumEncoders      = 4;
  static constexpr size_t kNumKeyence       = 2;
  static constexpr size_t kNumBrakePressure = 2;

  TemperatureData temperature;
  AmbientPressureData ambient_pressure;

  DataPoint<std::array<ImuData, kNumImus>> imu;
  std::array<CounterData, kNumEncoders> wheel_encoders;
  std::array<CounterData, kNumKeyence> keyence_stripe_counters;
  bool high_power_off = false;  // true if all SSRs are not in HP
};

struct BatteryData {
  static constexpr int kNumCells = 36;
  uint16_t voltage;             // dV
  int16_t current;              // dA
  uint8_t charge;               // %
  uint8_t average_temperature;  // C

  // below only for HighPowerBms! Value for BMSLP = 0
  uint16_t cell_voltage[kNumCells];  // mV
  int8_t low_temperature;            // C
  int8_t high_temperature;           // C
  uint16_t low_voltage_cell;         // mV
  uint16_t high_voltage_cell;        // mV
  bool imd_fault;
};

struct Batteries : public Module {
  static constexpr int kNumLPBatteries = 2;
  static constexpr int kNumHPBatteries = 1;

  std::array<BatteryData, kNumLPBatteries> low_power_batteries;
  std::array<BatteryData, kNumHPBatteries> high_power_batteries;
};

struct EmergencyBrakes : public Module {
  static constexpr int kBrakeCommandWaitTime = 1000;  // milliseconds
  static constexpr int kNumBrakes            = 2;
  bool brakes_retracted[kNumBrakes]          = {false};  // true if brakes retract
};

// -------------------------------------------------------------------------------------------------
// Motor data
// -------------------------------------------------------------------------------------------------

struct Motors : public Module {
  static constexpr int kNumMotors              = 4;
  static constexpr uint8_t kMaximumTemperature = 150;
  static constexpr uint16_t kMaximumCurrent    = 1500;  // mA
  std::array<uint32_t, kNumMotors> rpms        = {{0, 0, 0, 0}};
};

// -------------------------------------------------------------------------------------------------
// Telemetry data
// -------------------------------------------------------------------------------------------------

struct Telemetry : public Module {
  bool calibrate_command       = false;
  bool launch_command          = false;
  bool shutdown_command        = false;
  bool service_propulsion_go   = false;
  bool emergency_stop_command  = false;
  bool nominal_braking_command = true;
};

// -------------------------------------------------------------------------------------------------
// State Machine States
// -------------------------------------------------------------------------------------------------
enum class State {
  kIdle,
  kPreCalibrating,
  kCalibrating,
  kPreReady,
  kReady,
  kAccelerating,
  kCruising,
  kPreBraking,
  kNominalBraking,
  kFailurePreBraking,
  kEmergencyBraking,
  kFailureStopped,
  kFinished,
  kInvalid
};

std::optional<std::string> stateToString(const State state);
std::optional<State> stateFromString(const std::string &state_name);

struct StateMachine {
  bool critical_failure;
  State current_state;
};

// -------------------------------------------------------------------------------------------------
// Common Data structure/class
// -------------------------------------------------------------------------------------------------
/**
 * @brief      A singleton class managing the data exchange between sub-team
 * threads.
 */
class Data {
 public:
  /**
   * @brief      Always returns a reference to the only instance of `Data`.
   */
  static Data &getInstance();

  /**
   * @brief      Retrieves data related to the state machine. Data has high priority.
   */
  StateMachine getStateMachineData();

  /**
   * @brief      Should be called by state machine team to update data.
   */
  void setStateMachineData(const StateMachine &sm_data);

  /**
   * @brief      Retrieves data produced by navigation sub-team.
   */
  Navigation getNavigationData();

  /**
   * @brief      Should be called by navigation sub-team whenever they have new data.
   */
  void setNavigationData(const Navigation &nav_data);

  /**
   * @brief      Retrieves data from all sensors
   */
  Sensors getSensorsData();

  /**
   * @brief retrieves imu data from Sensors
   */
  DataPoint<std::array<ImuData, Sensors::kNumImus>> getSensorsImuData();

  /**
   * @brief retrieves encoder data from Sensors
   */
  std::array<CounterData, Sensors::kNumEncoders> getSensorsWheelEncoderData();

  /**
   * @brief retrieves gpio_counter data from Sensors
   */
  std::array<CounterData, Sensors::kNumKeyence> getSensorsKeyenceData();

  /**
   * @brief      Should be called to update sensor data.
   */
  void setSensorsData(const Sensors &sensors_data);

  /**
   * @brief      Should be called to update sensor imu data.
   */
  void setSensorsImuData(const DataPoint<std::array<ImuData, Sensors::kNumImus>> &imu);

  /**
   * @brief      Should be called to update sensor encoder data.
   */
  void setSensorsWheelEncoderData(const std::array<CounterData, Sensors::kNumEncoders> &imu);

  /**
   * @brief      Should be called to update sensor keyence data.
   */
  void setSensorsKeyenceData(
    const std::array<CounterData, Sensors::kNumKeyence> &keyence_stripe_counter);

  /**
   * @brief      Retrieves data from the batteries.
   */
  Batteries getBatteriesData();

  /**
   * @brief      Should be called to update battery data
   */
  void setBatteriesData(const Batteries &batteries_data);

  /**
   * @brief      Retrieves data from the emergency brakes.
   */
  EmergencyBrakes getEmergencyBrakesData();

  /**
   * @brief      Should be called to update emergency brakes data
   */
  void setEmergencyBrakesData(const EmergencyBrakes &emergency_brakes_data);

  /**
   * @brief      Retrieves data produced by each of the four motors.
   */
  Motors getMotorData();

  /**
   * @brief      Should be called to update motor data.
   */
  void setMotorData(const Motors &motor_data);

  /**
   * @brief      Retrieves data on whether stop/kill power commands have been issued.
   */
  Telemetry getTelemetryData();

  /**
   * @brief      Should be called to update communications data.
   */
  void setTelemetryData(const Telemetry &telemetry_data);

 private:
  StateMachine state_machine_;
  Navigation navigation_;
  Sensors sensors_;
  Motors motors_;
  Batteries batteries_;
  Telemetry telemetry_;
  EmergencyBrakes emergency_brakes_;

  // locks for data substructures
  utils::concurrent::Lock lock_state_machine_;
  utils::concurrent::Lock lock_navigation_;
  utils::concurrent::Lock lock_sensors_;
  utils::concurrent::Lock lock_motors_;

  utils::concurrent::Lock lock_telemetry_;
  utils::concurrent::Lock lock_batteries_;
  utils::concurrent::Lock lock_emergency_brakes_;

  Data() {}

 public:
  Data(const Data &) = delete;
  Data &operator=(const Data &) = delete;
  Data(Data &&)                 = delete;
  Data &operator=(Data &&) = delete;
};

}  // namespace hyped::data
