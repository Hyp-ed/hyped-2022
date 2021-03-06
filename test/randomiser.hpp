#pragma once

#include <fcntl.h>
#include <stdlib.h>

#include <random>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <data/data.hpp>
#include <state_machine/state.hpp>
#include <state_machine/transitions.hpp>
#include <utils/logger.hpp>

namespace hyped::testing {

//---------------------------------------------------------------------------
// Randomiser
//---------------------------------------------------------------------------

/**
 * Randomises entries in data that are used by various modules to check
 * different conditions and to test state behaviour in different scenarios.
 */
class Randomiser {
 public:
  // Generates a random floating point number in the inverval [0.0, 1.0].
  static data::nav_t randomDecimal()
  {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<data::nav_t> distribution(0.0, 1.0);
    return distribution(generator);
  }

  static data::nav_t randomInRange(const data::nav_t lower, const data::nav_t upper)
  {
    static std::default_random_engine random_engine;
    std::uniform_real_distribution<> distribution(lower, upper);
    return distribution(random_engine);
  }

  // Randomises a module status to any of the possible values.
  static void randomiseModuleStatus(data::ModuleStatus &module_status)
  {
    // Randomises the module status.
    constexpr int num_statuses = 4;
    module_status              = static_cast<data::ModuleStatus>(rand() % num_statuses);
  }

  //---------------------------------------------------------------------------
  // Navigation data
  //---------------------------------------------------------------------------

  // Randomises the entries in a hyped::data::Navigation struct.
  static void randomiseNavigation(data::Navigation &nav_data)
  {
    randomiseModuleStatus(nav_data.module_status);

    // Generates a displacement length between 750 and 1749.
    nav_data.displacement = static_cast<data::nav_t>((rand() % 1000 + 750) + randomDecimal());

    // Generates a velocity length between -10 and 200.
    nav_data.velocity = static_cast<data::nav_t>((rand() % 201 - 10) + randomDecimal());

    // Generates an acceleration length between 75 and 174.
    nav_data.acceleration = static_cast<data::nav_t>((rand() % 100 + 75) + randomDecimal());

    // Generates an emergency braking distance length between 600 and 899.
    nav_data.emergency_braking_distance
      = static_cast<data::nav_t>((rand() % 300 + 600) + randomDecimal());

    // Generates a braking distance length between 600 and 899.
    // Initially declared braking distance = 750
    nav_data.braking_distance = static_cast<data::nav_t>((rand() % 300 + 600) + randomDecimal());
  }

  //---------------------------------------------------------------------------
  // Raw Sensor data
  //---------------------------------------------------------------------------

  // Randomises the entries in a hyped::data::ImuData struct.
  static void randomiseImuData(data::ImuData &imu_data)
  {
    for (size_t i = 0; i < 3; ++i) {
      imu_data.acc[i] = static_cast<data::nav_t>((rand() % 100 + 75) + randomDecimal());
    }
    for (size_t i = 0; i < 3; ++i) {
      imu_data.fifo.push_back(
        static_cast<data::NavigationVector>((rand() % 100 + 75) + randomDecimal()));
    }
  }

  // Randomises the entries in a hyped::data::StripeCounter struct.
  static void randomiseCounter(data::CounterData &stripe_counter)
  {
    // Generates a count timestamp and value between 0 and 10.
    stripe_counter.timestamp = static_cast<uint32_t>(rand() % 11);
    stripe_counter.value     = static_cast<uint32_t>(rand() % 11);
  }

  // Randomises the entries in a hyped::data::TemperatureData struct.
  static void randomiseTemperatureData(data::TemperatureData &temp_data)
  {
    // Generates a temperature value between 0 and 99 C.
    temp_data.temperature = static_cast<uint8_t>(rand() % 100);
  }

  // Randomises the entries in a hyped::data::Sensors struct.
  static void randomiseSensorsData(data::Sensors &sensors_data)
  {
    randomiseModuleStatus(sensors_data.module_status);

    sensors_data.imu.timestamp = static_cast<uint32_t>(rand() % 11);
    for (auto &sensors_data : sensors_data.imu.value) {
      randomiseImuData(sensors_data);
    }
    for (auto &sensors_data : sensors_data.wheel_encoders) {
      randomiseCounter(sensors_data);
    }
  }

  //---------------------------------------------------------------------------
  // Battery data
  //---------------------------------------------------------------------------

  // Randomises the entries in a hyped::data::BatteryData struct.
  static void randomiseBatteryData(data::BatteryData &battery_data)
  {
    // Generates a voltage data between 0 and 499.
    battery_data.voltage = static_cast<uint16_t>(rand() % 500);

    // Generates a current data between 0 and 499.
    battery_data.current = static_cast<int16_t>(rand() % 500);

    // Generates a charge percentage data between 0 and 100.
    battery_data.charge = static_cast<uint8_t>(rand() % 101);

    // Generates an average temperature data between 0 and 99 C.
    battery_data.average_temperature = static_cast<int8_t>(rand() % 100);

    // Below only for HighPowerBms! Value for BMSLP = 0
    // Generates a cell voltage data between 0 and 50000 mV.
    for (size_t i = 0; i < 36; ++i) {
      battery_data.cell_voltage[i] = static_cast<uint16_t>((rand() % 500) * 100);
    }

    // Generates a low temperature data between 0 and 99 C.
    battery_data.low_temperature = static_cast<int8_t>(rand() % 100);

    // Generates a high temperature data between 0 and 99 C.
    battery_data.high_temperature = static_cast<int8_t>(rand() % 100);

    // Generates a low cell voltage data between 0 and 50000 mV.
    battery_data.low_voltage_cell = static_cast<uint16_t>((rand() % 500) * 100);

    // Generates a high cell voltage data between 0 and 50000 mV.
    battery_data.high_voltage_cell = static_cast<uint16_t>((rand() % 500) * 100);

    // Generates a random bool value for IMD fault.
    battery_data.insulation_monitoring_device_fault = static_cast<bool>(rand() > (RAND_MAX / 2));
  }

  // Randomises the entries in a hyped::data::Batteries struct.
  static void randomiseBatteriesData(data::FullBatteryData &batteries_data)
  {
    randomiseModuleStatus(batteries_data.module_status);

    for (auto &battery_data : batteries_data.high_power_batteries) {
      randomiseBatteryData(battery_data);
    }
    for (auto &battery_data : batteries_data.low_power_batteries) {
      randomiseBatteryData(battery_data);
    }
  }

  //---------------------------------------------------------------------------
  // Emergency Brakes data
  //---------------------------------------------------------------------------

  // Randomises the entries in a hyped::data::Brakes struct.
  static void randomiseBrakes(data::Brakes &brakes_data)
  {
    randomiseModuleStatus(brakes_data.module_status);

    for (size_t i = 0; i < brakes_data.kNumBrakes; ++i) {
      brakes_data.brakes_retracted[i] = static_cast<bool>(rand() > (RAND_MAX / 2));
    }
  }

  //---------------------------------------------------------------------------
  // Motor data
  //---------------------------------------------------------------------------

  // Randomises the entries in a hyped::data::Motors struct.
  static void randomiseMotors(data::Motors &motors_data)
  {
    randomiseModuleStatus(motors_data.module_status);

    // Generates a RPM data between 0 and 199 for all 4 motors.
    for (size_t i = 0; i < motors_data.kNumMotors; ++i) {
      motors_data.rpms[i] = static_cast<uint32_t>(rand() % 200);
    }
  }

  //---------------------------------------------------------------------------
  // Telemetry data
  //---------------------------------------------------------------------------

  // Randomises the entries in a hyped::data::Telemetry struct.
  static void randomiseTelemetry(data::Telemetry &telemetry_data)
  {
    randomiseModuleStatus(telemetry_data.module_status);

    // Generates a random bool value for all telemetry commands.
    telemetry_data.calibrate_command       = static_cast<bool>(rand() > (RAND_MAX / 2));
    telemetry_data.launch_command          = static_cast<bool>(rand() > (RAND_MAX / 2));
    telemetry_data.shutdown_command        = static_cast<bool>(rand() > (RAND_MAX / 2));
    telemetry_data.service_propulsion_go   = static_cast<bool>(rand() > (RAND_MAX / 2));
    telemetry_data.emergency_stop_command  = static_cast<bool>(rand() > (RAND_MAX / 2));
    telemetry_data.nominal_braking_command = static_cast<bool>(rand() > (RAND_MAX / 2));
  }

  //---------------------------------------------------------------------------
  // State Machine States
  //---------------------------------------------------------------------------

  // Randomises the entries in a hyped::data::StateMachine struct.
  static void randomiseStateMachine(data::StateMachine &stm_data)
  {
    stm_data.critical_failure = static_cast<bool>(rand() > (RAND_MAX / 2));
  }
};
}  // namespace hyped::testing
