#include <string>

#include "data/data.hpp"
#include "gtest/gtest.h"
#include "telemetry/writer.hpp"

using namespace hyped::telemetry;
using namespace hyped::data;

Data &data_ = Data::getInstance();

/**
 * Tests packing of data in CDS to json
 */
struct WriterPackData : public ::testing::Test {
  // ---- Error messages -------
  const std::string telemetry_packing_error  = "Telemetry json does not match expected output.";
  const std::string navigation_packing_error = "Navigation json does not match expected output.";
  const std::string sensors_packing_error    = "Sensors json does not match expected output.";
  const std::string motor_packing_error      = "Motor json does not match expected output.";
  const std::string state_machine_packing_error
    = "State machine json does not match expected output.";
  const std::string battery_packing_error = "Battery json does not match expected output.";

 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(WriterPackData, packsTelemetryData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  writer.packTelemetryData();
  writer.end();
  std::string actualJson   = writer.getString();
  std::string expectedJson = "INSERT JSON HERE";  // TODO
  ASSERT_EQ(actualJson, expectedJson) << telemetry_packing_error;
}

TEST_F(WriterPackData, packsNavigationData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  writer.packNavigationData();
  writer.end();
  std::string actualJson   = writer.getString();
  std::string expectedJson = "INSERT JSON HERE";  // TODO
  ASSERT_EQ(actualJson, expectedJson) << navigation_packing_error;
}

TEST_F(WriterPackData, packsSensorsData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  writer.packSensorsData();
  writer.end();
  std::string actualJson   = writer.getString();
  std::string expectedJson = "INSERT JSON HERE";  // TODO
  ASSERT_EQ(actualJson, expectedJson) << sensors_packing_error;
}

TEST_F(WriterPackData, packsMotorData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  writer.packMotorData();
  writer.end();
  std::string actualJson   = writer.getString();
  std::string expectedJson = "INSERT JSON HERE";  // TODO
  ASSERT_EQ(actualJson, expectedJson) << motor_packing_error;
}

TEST_F(WriterPackData, packsStateMachineData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  writer.packStateMachineData();
  writer.end();
  std::string actualJson   = writer.getString();
  std::string expectedJson = "INSERT JSON HERE";  // TODO
  ASSERT_EQ(actualJson, expectedJson) << state_machine_packing_error;
}

TEST_F(WriterPackData, packsBatteryData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  // writer.packBattery();
  writer.end();
  std::string actualJson   = writer.getString();
  std::string expectedJson = "INSERT JSON HERE";  // TODO
  ASSERT_EQ(actualJson, expectedJson) << battery_packing_error;
}
