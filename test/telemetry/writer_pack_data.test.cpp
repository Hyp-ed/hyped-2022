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

TEST_F(WriterPackData, packsTelemetryData)
{
  // TODO initialise CDS with values
  Writer writer(data_);
  writer.start();
  writer.packTelemetryData();
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
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
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
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
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
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
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
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
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
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
  const std::string expectedJson = "INSERT JSON HERE";  // TODO
  ASSERT_EQ(actualJson, expectedJson) << "Battery json does not match expected output.";
}
