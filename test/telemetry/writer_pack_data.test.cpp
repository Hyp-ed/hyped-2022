#include <string>

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
  data_.setNavigationData(navigation_data);
}

void setSensorsData()
{
  Sensors sensors_data = data_.getSensorsData();
  data_.setSensorsData(sensors_data);
}

void setMotorData()
{
  Motors motors_data = data_.getMotorData();
  data_.setMotorData(motors_data);
}

void setStateMachineData()
{
  StateMachine state_machine_data = data_.getStateMachineData();
  data_.setStateMachineData(state_machine_data);
}

void setBatteryData()
{
  Batteries battery_data = data_.getBatteriesData();
  data_.setBatteriesData(battery_data);
}

TEST_F(WriterPackData, packsTelemetryData)
{
  setTelemetryData();
  Writer writer(data_);
  writer.start();
  writer.packTelemetryData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
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
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
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
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
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
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
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
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "State machine json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "State machine json invalid.";
}

TEST_F(WriterPackData, packsBatteryData)
{
  setBatteryData();
  Writer writer(data_);
  writer.start();
  // writer.packBattery();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Battery json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "Battery json invalid.";
}
