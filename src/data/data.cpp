#include "data.hpp"

namespace hyped {

// imports
using utils::concurrent::ScopedLock;

namespace data {

static const std::unordered_map<State, std::string> state_names = {
  {State::kIdle, "Idle"},
  {State::kPreCalibrating, "PreCalibrating"},
  {State::kCalibrating, "Calibrating"},
  {State::kReady, "Ready"},
  {State::kAccelerating, "Accelerating"},
  {State::kCruising, "Cruising"},
  {State::kPreBraking, "PreBraking"},
  {State::kNominalBraking, "NominalBraking"},
  {State::kFailurePreBraking, "FailurePreBraking"},
  {State::kFailureBraking, "EmergencyBraking"},
  {State::kFailureStopped, "FailureStopped"},
  {State::kFinished, "Finished"},
  {State::kInvalid, "Invalid"},
};

static const std::unordered_map<std::string, State> states_by_name = {
  {"Idle", State::kIdle},
  {"PreCalibrating", State::kPreCalibrating},
  {"Calibrating", State::kCalibrating},
  {"Ready", State::kReady},
  {"Accelerating", State::kAccelerating},
  {"Cruising", State::kCruising},
  {"PreBraking", State::kPreBraking},
  {"NominalBraking", State::kNominalBraking},
  {"FailurePreBraking", State::kFailurePreBraking},
  {"EmergencyBraking", State::kFailureBraking},
  {"FailureStopped", State::kFailureStopped},
  {"Finished", State::kFinished},
  {"Invalid", State::kInvalid},
};

std::optional<std::string> stateToString(const State state)
{
  const auto it = state_names.find(state);
  if (it == state_names.end()) { return std::nullopt; }
  return it->second;
}

std::optional<State> stateFromString(const std::string &state_name)
{
  const auto it = states_by_name.find(state_name);
  if (it == states_by_name.end()) { return std::nullopt; }
  return it->second;
}

Data &Data::getInstance()
{
  static Data d;
  return d;
}

StateMachine Data::getStateMachineData()
{
  ScopedLock L(&lock_state_machine_);
  return state_machine_;
}

void Data::setStateMachineData(const StateMachine &sm_data)
{
  ScopedLock L(&lock_state_machine_);
  state_machine_ = sm_data;
}

Navigation Data::getNavigationData()
{
  ScopedLock L(&lock_navigation_);
  return navigation_;
}

void Data::setNavigationData(const Navigation &nav_data)
{
  ScopedLock L(&lock_navigation_);
  navigation_ = nav_data;
}

Sensors Data::getSensorsData()
{
  ScopedLock L(&lock_sensors_);
  return sensors_;
}

DataPoint<std::array<ImuData, Sensors::kNumImus>> Data::getSensorsImuData()
{
  ScopedLock L(&lock_sensors_);
  return sensors_.imu;
}

std::array<CounterData, Sensors::kNumEncoders> Data::getSensorsWheelEncoderData()
{
  ScopedLock L(&lock_sensors_);
  return sensors_.wheel_encoders;
}

int Data::getTemperature()
{
  ScopedLock L(&lock_temp_);
  return temperature_;
}

void Data::setTemperature(const int &temp)
{
  ScopedLock L(&lock_temp_);
  temperature_ = temp;
}

void Data::setSensorsData(const Sensors &sensors_data)
{
  ScopedLock L(&lock_sensors_);
  sensors_ = sensors_data;
}

void Data::setSensorsImuData(const DataPoint<std::array<ImuData, Sensors::kNumImus>> &imu)
{
  ScopedLock L(&lock_sensors_);
  sensors_.imu = imu;
}

void Data::setSensorsWheelEncoderData(const std::array<CounterData, Sensors::kNumEncoders> &encoder)
{
  ScopedLock L(&lock_sensors_);
  sensors_.wheel_encoders = encoder;
}

FullBatteryData Data::getBatteriesData()
{
  ScopedLock L(&lock_batteries_);
  return batteries_;
}

void Data::setBatteriesData(const FullBatteryData &batteries_data)
{
  ScopedLock L(&lock_batteries_);
  batteries_ = batteries_data;
}

EmergencyBrakes Data::getEmergencyBrakesData()
{
  ScopedLock L(&lock_emergency_brakes_);
  return emergency_brakes_;
}

void Data::setEmergencyBrakesData(const EmergencyBrakes &emergency_brakes_data)
{
  ScopedLock L(&lock_emergency_brakes_);
  emergency_brakes_ = emergency_brakes_data;
}

Motors Data::getMotorData()
{
  ScopedLock L(&lock_motors_);
  return motors_;
}

void Data::setMotorData(const Motors &motor_data)
{
  ScopedLock L(&lock_motors_);
  motors_ = motor_data;
}

Telemetry Data::getTelemetryData()
{
  ScopedLock L(&lock_telemetry_);
  return telemetry_;
}

void Data::setTelemetryData(const Telemetry &telemetry_data)
{
  ScopedLock L(&lock_telemetry_);
  telemetry_ = telemetry_data;
}

}  // namespace data
}  // namespace hyped
