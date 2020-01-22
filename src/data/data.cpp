/*
 * Organisation: HYPED
 * Date:
 * Description:
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

#include "data/data.hpp"

namespace hyped {

// imports
using utils::concurrent::ScopedLock;

namespace data {

const char* states[num_states] = {
  "Idle",
  "Calibrating",
  "Ready",
  "Accelerating",
  "NominalBraking",
  "EmergencyBraking",
  "RunComplete",
  "FailureStopped",
  "Exiting",
  "Finished",
};

Data& Data::getInstance()
{
  static Data d;
  return d;
}

StateMachine Data::getStateMachineData()
{
  ScopedLock L(&lock_state_machine_);
  return state_machine_;
}

void Data::setStateMachineData(const StateMachine& sm_data)
{
  ScopedLock L(&lock_state_machine_);
  state_machine_ = sm_data;
}

Navigation Data::getNavigationData()
{
  ScopedLock L(&lock_navigation_);
  return navigation_;
}

void Data::setNavigationData(const Navigation& nav_data)
{
  ScopedLock L(&lock_navigation_);
  navigation_ = nav_data;
}

Sensors Data::getSensorsData()
{
  ScopedLock L(&lock_sensors_);
  return sensors_;
}

DataPoint<array<ImuData, Sensors::kNumImus>> Data::getSensorsImuData()
{
  ScopedLock L(&lock_sensors_);
  return sensors_.imu;
}

array<StripeCounter, Sensors::kNumKeyence> Data::getSensorsKeyenceData()
{
  ScopedLock L(&lock_sensors_);
  return sensors_.keyence_stripe_counter;
}

int Data::getTemperature()
{
  ScopedLock L(&lock_temp_);
  return temperature_;
}

void Data::setTemperature(const int& temp)
{
  ScopedLock L(&lock_temp_);
  temperature_ = temp;
}

void Data::setSensorsData(const Sensors& sensors_data)
{
  ScopedLock L(&lock_sensors_);
  sensors_ = sensors_data;
}

void Data::setSensorsImuData(const DataPoint<array<ImuData, Sensors::kNumImus>>& imu)
{
  ScopedLock L(&lock_sensors_);
  sensors_.imu = imu;
}

void Data::setSensorsKeyenceData(const array<StripeCounter, Sensors::kNumKeyence>& keyence_stripe_counter) //NOLINT
{
  ScopedLock L(&lock_sensors_);
  sensors_.keyence_stripe_counter = keyence_stripe_counter;
}

Batteries Data::getBatteriesData()
{
  ScopedLock L(&lock_batteries_);
  return batteries_;
}

void Data::setBatteriesData(const Batteries& batteries_data)
{
  ScopedLock L(&lock_batteries_);
  batteries_ = batteries_data;
}

EmergencyBrakes Data::getEmergencyBrakesData()
{
  ScopedLock L(&lock_emergency_brakes_);
  return emergency_brakes_;
}

void Data::setEmergencyBrakesData(const EmergencyBrakes& emergency_brakes_data)
{
  ScopedLock L(&lock_emergency_brakes_);
  emergency_brakes_ = emergency_brakes_data;
}

Motors Data::getMotorData()
{
  ScopedLock L(&lock_motors_);
  return motors_;
}

void Data::setMotorData(const Motors& motor_data)
{
  ScopedLock L(&lock_motors_);
  motors_ = motor_data;
}

Telemetry Data::getTelemetryData()
{
  ScopedLock L(&lock_telemetry_);
  return telemetry_;
}

void Data::setTelemetryData(const Telemetry& telemetry_data)
{
  ScopedLock L(&lock_telemetry_);
  telemetry_ = telemetry_data;
}

}}  // namespace data::hyped

