#include "data/data.hpp"
#include "gtest/gtest.h"
#include "telemetry/writer.hpp"

using namespace hyped::telemetry;
using namespace hyped::data;

/**
 * Tests packing of data in CDS to json
 */
struct WriterPackData : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

Data &data_ = Data::getInstance();

void setTelemetryData()
{
  Telemetry telemetry_data = data_.getTelemetryData();

  telemetry_data.calibrate_command       = false;
  telemetry_data.emergency_stop_command  = false;
  telemetry_data.launch_command          = false;
  telemetry_data.nominal_braking_command = false;
  telemetry_data.service_propulsion_go   = false;
  telemetry_data.shutdown_command        = false;
  telemetry_data.module_status           = ModuleStatus::kCriticalFailure;

  data_.setTelemetryData(telemetry_data);
}

void setNavigationData()
{
  Navigation navigation_data = data_.getNavigationData();

  navigation_data.acceleration               = 10.0;
  navigation_data.braking_distance           = 20.0;
  navigation_data.displacement               = 100.0;
  navigation_data.emergency_braking_distance = 50.0;
  navigation_data.velocity                   = 5.0;
  navigation_data.module_status              = ModuleStatus::kReady;

  data_.setNavigationData(navigation_data);
}

void setSensorsData()
{
  Sensors sensors_data        = data_.getSensorsData();
  Batteries batteries_data    = data_.getBatteriesData();
  EmergencyBrakes brakes_data = data_.getEmergencyBrakesData();

  sensors_data.module_status   = ModuleStatus::kInit;
  brakes_data.module_status    = ModuleStatus::kReady;
  batteries_data.module_status = ModuleStatus::kStart;
  data_.setTemperature(20);
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

  data_.setSensorsData(sensors_data);
}

void setMotorData()
{
  Motors motors_data = data_.getMotorData();

  motors_data.module_status = ModuleStatus::kCriticalFailure;
  for (int16_t i = 0; i < motors_data.kNumMotors; ++i) {
    motors_data.rpms[i] = 150;
  }

  data_.setMotorData(motors_data);
}

void setStateMachineData()
{
  StateMachine state_machine_data = data_.getStateMachineData();

  state_machine_data.critical_failure = false;
  state_machine_data.current_state    = kAccelerating;

  data_.setStateMachineData(state_machine_data);
}

TEST_F(WriterPackData, packsId)
{
  Writer writer(data_);
  writer.start();
  writer.packId(5);
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "ID json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "ID json invalid.";
}

TEST_F(WriterPackData, packsTelemetryData)
{
  setTelemetryData();
  Writer writer(data_);
  writer.start();
  writer.packTelemetryData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = {"calibrate": bool,"emergency_stop": bool, "launch": bool , "nominal_breaking": bool ,"service_propulsion_go":bool,"shutdown":bool,"telemetry_status":String};  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Telemetry json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "Telemetry json invalid.";
}

TEST_F(WriterPackData, packsNavigationData)
{
  setNavigationData();
  Writer writer(data_);
  writer.start();
  writer.packNavigationData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = {"braking_distance": 750, "displacement":1250,"emergency_braking_distance":20,"velocity":100,"acceleration":8};  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Navigation json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "Navigation json invalid.";
}

TEST_F(WriterPackData, packsSensorsData)
{
  setSensorsData();
  Writer writer(data_);
  writer.start();
  writer.packSensorsData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = {[{"lp_battery":{"average_temp":int, "voltage":int,"current":int,"charge":int,"low_temp":int,"high_temp":int,"low_voltage_cell":int,"high_voltage_cell":int,"imd_fault": false}},{"lp_battery":{"average_temp":int, "voltage":int,"current":int,"charge":int,"low_temp":int,"high_temp":int,"low_voltage_cell":int,"high_voltage_cell":int,"imd_fault": false}}],[{"hp_battery":{"average_temp":int, "voltage":int,"current":int,"charge":int,"low_temp":int,"high_temp":int,"low_voltage_cell":int,"high_voltage_cell":int,"imd_fault": false}}], "brakes_retracted": bool,"temperature":int,"brake status": String,"sensors_status":bool, "batteries_status": bool};  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Sensors json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "Sensors json invalid.";
}

TEST_F(WriterPackData, packsMotorData)
{
  setMotorData();
  Writer writer(data_);
  writer.start();
  writer.packMotorData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = {"motor_rpms":[0,0,0,0],"motors_status":String} ;  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Motor json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "Motor json invalid.";
}

TEST_F(WriterPackData, packsStateMachineData)
{
  setStateMachineData();
  Writer writer(data_);
  writer.start();
  writer.packStateMachineData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = {"critical_failure": bool,"current_state": String};  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "State machine json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "State machine json invalid.";
}

TEST_F(WriterPackData, packsBatteryData)
{
  setSensorsData();
  Writer writer(data_);
  writer.start();
  // writer.packBattery();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson =  <value>: {"average_temp":int, "voltage":int,"current":int,"charge":int,"low_temp":int,"high_temp":int,"low_voltage_cell":int,"high_voltage_cell":int,"imd_fault": bool};  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Battery json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "Battery json invalid.";
}
