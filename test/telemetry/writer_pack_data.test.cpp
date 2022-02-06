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

TEST_F(WriterPackData, packsTelemetryData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  writer.packTelemetryData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = {"calibrate": bool,"emergency_stop": bool, "launch": bool , "nominal_breaking": bool ,"service_propulsion_go":bool,"shutdown":bool,"telemetry_status":String};  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Telemetry json does not match expected output.";
}

TEST_F(WriterPackData, packsNavigationData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  writer.packNavigationData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = {"braking_distance": 750, "displacement":1250,"emergency_braking_distance":20,"velocity":100,"acceleration":8};  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Navigation json does not match expected output.";
}

TEST_F(WriterPackData, packsSensorsData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  writer.packSensorsData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = {[{"lp_battery":{"average_temp":int, "voltage":int,"current":int,"charge":int,"low_temp":int,"high_temp":int,"low_voltage_cell":int,"high_voltage_cell":int,"imd_fault": false}},{"lp_battery":{"average_temp":int, "voltage":int,"current":int,"charge":int,"low_temp":int,"high_temp":int,"low_voltage_cell":int,"high_voltage_cell":int,"imd_fault": false}}],[{"hp_battery":{"average_temp":int, "voltage":int,"current":int,"charge":int,"low_temp":int,"high_temp":int,"low_voltage_cell":int,"high_voltage_cell":int,"imd_fault": false}}], "brakes_retracted": bool,"temperature":int,"brake status": String,"sensors_status":bool, "batteries_status": bool};  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Sensors json does not match expected output.";
}

TEST_F(WriterPackData, packsMotorData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  writer.packMotorData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = {"motor_rpms":[0,0,0,0],"motors_status":String} ;  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Motor json does not match expected output.";
}

TEST_F(WriterPackData, packsStateMachineData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  writer.packStateMachineData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = {"critical_failure": bool,"current_state": String};  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "State machine json does not match expected output.";
}

TEST_F(WriterPackData, packsBatteryData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  // writer.packBattery();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson =  <value>: {"average_temp":int, "voltage":int,"current":int,"charge":int,"low_temp":int,"high_temp":int,"low_voltage_cell":int,"high_voltage_cell":int,"imd_fault": bool};  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Battery json does not match expected output.";
}
