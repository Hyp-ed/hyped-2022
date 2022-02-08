#include "data/data.hpp"
#include "gtest/gtest.h"
#include "telemetry/writer.hpp"

namespace hyped {

/**
 * Tests packing of data in CDS to json
 */
struct WriterPackData : public ::testing::Test {
 protected:
  data::Data &data = data::Data::getInstance();
  void SetUp() {}
  void TearDown() {}
};

TEST_F(WriterPackData, packsId)
{
  telemetry::Writer writer;
  writer.start();
  writer.packId(5);
  writer.end();
  const std::string actual_json   = writer.getString();
  const std::string expected_json = "{\"id\":5}";
  ASSERT_EQ(actual_json, expected_json) << "ID json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "ID json invalid.";
}

TEST_F(WriterPackData, packsTelemetryData)
{
  data::Telemetry telemetry_data = data.getTelemetryData();

  telemetry_data.calibrate_command       = false;
  telemetry_data.emergency_stop_command  = false;
  telemetry_data.launch_command          = false;
  telemetry_data.nominal_braking_command = false;
  telemetry_data.service_propulsion_go   = false;
  telemetry_data.shutdown_command        = false;
  telemetry_data.module_status           = data::ModuleStatus::kCriticalFailure;

  data.setTelemetryData(telemetry_data);

  telemetry::Writer writer;
  writer.start();
  writer.packTelemetryData();
  writer.end();
  const std::string actual_json = writer.getString();
  const std::string expected_json
    = "{\"telemetry\":{\"calibrate\":false,\"emergency_stop\":false,\"launch\":false,\"nominal_"
      "breaking\":false,\"service_propulsion_go\":false,\"shutdown\":false,\"telemetry_status\":"
      "\"CRITICAL_FAILURE\"}}";
  ASSERT_EQ(actual_json, expected_json) << "Telemetry json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "Telemetry json invalid.";
}

TEST_F(WriterPackData, packsNavigationData)
{
  data::Navigation navigation_data = data.getNavigationData();

  navigation_data.acceleration               = 10.0;
  navigation_data.braking_distance           = 20.0;
  navigation_data.displacement               = 100.0;
  navigation_data.emergency_braking_distance = 50.0;
  navigation_data.velocity                   = 5.0;
  navigation_data.module_status              = data::ModuleStatus::kReady;

  data.setNavigationData(navigation_data);

  telemetry::Writer writer;
  writer.start();
  writer.packNavigationData();
  writer.end();
  const std::string actual_json = writer.getString();
  const std::string expected_json
    = "{\"navigation\":{\"braking_distance\":20,\"displacement\":100,\"emergency_braking_"
      "distance\":50,\"velocity\":5,\"acceleration\":10,\"navigation_status\":\"READY\"}}";
  ASSERT_EQ(actual_json, expected_json) << "Navigation json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "Navigation json invalid.";
}

TEST_F(WriterPackData, packsSensorsData)
{
  data::Sensors sensors_data        = data.getSensorsData();
  data::Batteries batteries_data    = data.getBatteriesData();
  data::EmergencyBrakes brakes_data = data.getEmergencyBrakesData();

  sensors_data.module_status   = data::ModuleStatus::kInit;
  brakes_data.module_status    = data::ModuleStatus::kReady;
  batteries_data.module_status = data::ModuleStatus::kStart;
  data.setTemperature(20);
  for (int16_t i = 0; i < batteries_data.kNumHPBatteries; ++i) {
    batteries_data.high_power_batteries[i].average_temperature = 8;
    batteries_data.high_power_batteries[i].charge              = 3;
    batteries_data.high_power_batteries[i].current             = 10;
    batteries_data.high_power_batteries[i].high_temperature    = 16;
    batteries_data.high_power_batteries[i].high_voltage_cell   = 24;
    batteries_data.high_power_batteries[i].imd_fault           = false;
    batteries_data.high_power_batteries[i].low_temperature     = 13;
    batteries_data.high_power_batteries[i].low_voltage_cell    = 5;
    batteries_data.high_power_batteries[i].voltage             = 12;
  }
  for (int16_t i = 0; i < batteries_data.kNumLPBatteries; ++i) {
    batteries_data.low_power_batteries[i].average_temperature = 8;
    batteries_data.low_power_batteries[i].charge              = 3;
    batteries_data.low_power_batteries[i].current             = 10;
    batteries_data.low_power_batteries[i].high_temperature    = 16;
    batteries_data.low_power_batteries[i].high_voltage_cell   = 24;
    batteries_data.low_power_batteries[i].imd_fault           = false;
    batteries_data.low_power_batteries[i].low_temperature     = 13;
    batteries_data.low_power_batteries[i].low_voltage_cell    = 5;
    batteries_data.low_power_batteries[i].voltage             = 12;
  }

  data.setSensorsData(sensors_data);
  data.setEmergencyBrakesData(brakes_data);
  data.setBatteriesData(batteries_data);

  telemetry::Writer writer;
  writer.start();
  writer.packSensorsData();
  writer.end();
  const std::string actual_json = writer.getString();
  const std::string expected_json
    = "{\"sensors\":{\"lp_batteries\":[{\"average_temp\":8,\"voltage\":12,\"current\":10,"
      "\"charge\":3,\"low_temp\":13,\"high_temp\":16,\"low_voltage_cell\":5,\"high_voltage_cell\":"
      "24,\"imd_fault\":false},{\"average_temp\":8,\"voltage\":12,\"current\":10,\"charge\":3,"
      "\"low_temp\":13,\"high_temp\":16,\"low_voltage_cell\":5,\"high_voltage_cell\":24,\"imd_"
      "fault\":false}],\"hp_batteries\":[{\"average_temp\":8,\"voltage\":12,\"current\":10,"
      "\"charge\":3,\"low_temp\":13,\"high_temp\":16,\"low_voltage_cell\":5,\"high_voltage_cell\":"
      "24,\"imd_fault\":false}],\"brakes_retracted\":true,\"temperature\":20,\"brakes_status\":"
      "\"READY\",\"sensors_status\":\"INIT\",\"batteries_status\":\"START\"}}";
  ASSERT_EQ(actual_json, expected_json) << "Sensors json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "Sensors json invalid.";
}

TEST_F(WriterPackData, packsMotorData)
{
  data::Motors motors_data = data.getMotorData();

  motors_data.module_status = data::ModuleStatus::kCriticalFailure;
  for (int16_t i = 0; i < motors_data.kNumMotors; ++i) {
    motors_data.rpms[i] = 150;
  }

  data.setMotorData(motors_data);

  telemetry::Writer writer;
  writer.start();
  writer.packMotorData();
  writer.end();
  const std::string actual_json = writer.getString();
  const std::string expected_json
    = "{\"motors\":{\"motor_rpms\":[150,150,150,150],\"motors_status\":\"CRITICAL_FAILURE\"}}";
  ASSERT_EQ(actual_json, expected_json) << "Motor json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "Motor json invalid.";
}

TEST_F(WriterPackData, packsStateMachineData)
{
  data::StateMachine state_machine_data = data.getStateMachineData();

  state_machine_data.critical_failure = false;
  state_machine_data.current_state    = data::kAccelerating;

  data.setStateMachineData(state_machine_data);

  telemetry::Writer writer;
  writer.start();
  writer.packStateMachineData();
  writer.end();
  const std::string actual_json = writer.getString();
  const std::string expected_json
    = "{\"state_machine\":{\"critical_failure\":false,\"current_state\":\"ACCELERATING\"}}";
  ASSERT_EQ(actual_json, expected_json) << "State machine json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "State machine json invalid.";
}

}  // namespace hyped
