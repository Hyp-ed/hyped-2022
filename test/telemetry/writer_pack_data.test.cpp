#include "data/data.hpp"
#include "gtest/gtest.h"
#include "telemetry/writer.hpp"

using namespace hyped::telemetry;
using namespace hyped::data;
using namespace std;

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
  data_.setEmergencyBrakesData(brakes_data);
  data_.setBatteriesData(batteries_data);
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
  int id = 5;
  writer.start();
  writer.packId(id);
  writer.end();
  const std::string actualJson   = writer.getString();
  const std::string expectedJson = "{\"id\":" + std::to_string(id) + "}";
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
  const std::string actualJson = writer.getString();
  const std::string expectedJson
    = "{\"telemetry\":{\"calibrate\":false,\"emergency_stop\":false,\"launch\":false,\"nominal_"
      "breaking\":false,\"service_propulsion_go\":false,\"shutdown\":false,\"telemetry_status\":"
      "\"CRITICAL_FAILURE\"}}";
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
  const std::string actualJson = writer.getString();
  const std::string expectedJson
    = "{\"navigation\":{\"braking_distance\":20,\"displacement\":100,\"emergency_braking_"
      "distance\":50,\"velocity\":5,\"acceleration\":10,\"navigation_status\":\"READY\"}}";
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
  const std::string actualJson = writer.getString();
  const std::string expectedJson
    = "{\"sensors\":{\"lp_batteries\":[{\"average_temp\":8,\"voltage\":12,\"current\":10,"
      "\"charge\":3,\"low_temp\":13,\"high_temp\":16,\"low_voltage_cell\":5,\"high_voltage_cell\":"
      "24,\"imd_fault\":false},{\"average_temp\":8,\"voltage\":12,\"current\":10,\"charge\":3,"
      "\"low_temp\":13,\"high_temp\":16,\"low_voltage_cell\":5,\"high_voltage_cell\":24,\"imd_"
      "fault\":false}],\"hp_batteries\":[{\"average_temp\":8,\"voltage\":12,\"current\":10,"
      "\"charge\":3,\"low_temp\":13,\"high_temp\":16,\"low_voltage_cell\":5,\"high_voltage_cell\":"
      "24,\"imd_fault\":false}],\"brakes_retracted\":true,\"temperature\":20,\"brakes_status\":"
      "\"READY\",\"sensors_status\":\"INIT\",\"batteries_status\":\"START\"}}";
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
  const std::string actualJson = writer.getString();
  const std::string expectedJson
    = "{\"motors\":{\"motor_rpms\":[150,150,150,150],\"motors_status\":\"CRITICAL_FAILURE\"}}";
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
  const std::string actualJson = writer.getString();
  const std::string expectedJson
    = "{\"state_machine\":{\"critical_failure\":false,\"current_state\":\"ACCELERATING\"}}";
  ASSERT_EQ(actualJson, expectedJson) << "State machine json does not match expected output.";
  ASSERT_TRUE(writer.isValidJson()) << "State machine json invalid.";
}
