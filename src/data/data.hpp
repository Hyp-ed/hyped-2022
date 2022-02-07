#pragma once

#include "data_point.hpp"

#include <array>
#include <cstdint>
#include <vector>

#include <utils/concurrent/lock.hpp>
#include <utils/math/vector.hpp>

using std::array;
using std::vector;

namespace hyped {

// imports
using utils::concurrent::Lock;
using utils::math::Vector;

namespace data {

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
typedef float nav_t;
typedef Vector<nav_t, 3> NavigationVector;
struct Navigation : public Module {
  static constexpr nav_t kMaximumVelocity = 100;     // m/s
  static constexpr nav_t kRunLength       = 1250.0;  // m
  static constexpr nav_t kBrakingBuffer   = 20.0;    // m
  nav_t displacement;                                // m
  nav_t velocity;                                    // m/s
  nav_t acceleration;                                // m/s^2
  nav_t emergency_braking_distance;                  // m
  nav_t braking_distance = 750;                      // m
};

// -------------------------------------------------------------------------------------------------
// Raw Sensor data
// -------------------------------------------------------------------------------------------------
struct Sensor {
  bool operational;
};

struct ImuData : public Sensor {
  NavigationVector acc;

  std::vector<NavigationVector> fifo;
};

struct EncoderData : public Sensor {
  nav_t disp;
};

struct StripeCounter : public Sensor {
  DataPoint<uint32_t> count;
};

struct TemperatureData : public Sensor {
  uint16_t temp;  // C
};

struct PressureData : public Sensor {
  uint16_t pressure;  // mbar
};

struct Sensors : public Module {
  static constexpr int kNumImus     = 4;
  static constexpr int kNumEncoders = 4;
  static constexpr int kNumKeyence  = 2;

  TemperatureData temperature;
  PressureData pressure;

  DataPoint<array<ImuData, kNumImus>> imu;
  DataPoint<array<EncoderData, kNumEncoders>> encoder;
  array<StripeCounter, kNumKeyence> keyence_stripe_counter;
};

struct BatteryData {
  static constexpr int kNumCells = 36;
  uint16_t voltage;             // dV
  int16_t current;              // dA
  uint8_t charge;               // %
  uint8_t average_temperature;  // C

  // below only for BMSHP! Value for BMSLP = 0
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

  array<BatteryData, kNumLPBatteries> low_power_batteries;
  array<BatteryData, kNumHPBatteries> high_power_batteries;
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
enum State {
  kIdle,
  kPreCalibrating,
  kCalibrating,
  kReady,
  kAccelerating,
  kCruising,
  kNominalBraking,
  kEmergencyBraking,
  kFailureStopped,
  kFinished,
  kInvalid,
  num_states
};

extern const char *states[num_states];

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
  DataPoint<array<ImuData, Sensors::kNumImus>> getSensorsImuData();

  /**
   * @brief retrieves encoder data from Sensors
   */
  DataPoint<array<EncoderData, Sensors::kNumEncoders>> getSensorsEncoderData();

  /**
   * @brief retrieves gpio_counter data from Sensors
   */
  array<StripeCounter, Sensors::kNumKeyence> getSensorsKeyenceData();

  /**
   * @brief      Should be called to update sensor data.
   */
  void setSensorsData(const Sensors &sensors_data);
  /**
   * @brief      Should be called to update sensor imu data.
   */
  void setSensorsImuData(const DataPoint<array<ImuData, Sensors::kNumImus>> &imu);
  /**
   * @brief      Should be called to update sensor encoder data.
   */
  void setSensorsEncoderData(const DataPoint<array<EncoderData, Sensors::kNumEncoders>> &imu);
  /**
   * @brief      Should be called to update sensor keyence data.
   */
  void setSensorsKeyenceData(
    const array<StripeCounter, Sensors::kNumKeyence> &keyence_stripe_counter);  // NOLINT

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
  Lock lock_state_machine_;
  Lock lock_navigation_;
  Lock lock_sensors_;
  Lock lock_motors_;

  Lock lock_telemetry_;
  Lock lock_batteries_;
  Lock lock_emergency_brakes_;

  Data() {}

 public:
  Data(const Data &) = delete;
  Data &operator=(const Data &) = delete;
  Data(Data &&)                 = delete;
  Data &operator=(Data &&) = delete;
};

}  // namespace data
}  // namespace hyped
