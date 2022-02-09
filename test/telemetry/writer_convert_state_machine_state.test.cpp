#include <string>

#include "data/data.hpp"
#include "gtest/gtest.h"
#include "telemetry/writer.hpp"

namespace hyped::testing {

/**
 * Tests the conversion of State from Enum to String as required for GUI
 */
class WriterConvertStateMachineState : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(WriterConvertStateMachineState, convertsIdleState)
{
  std::string convertedState = telemetry::Writer::convertStateMachineState(data::State::kIdle);
  ASSERT_EQ(convertedState, "IDLE") << "Should convert Idle state.";
}

TEST_F(WriterConvertStateMachineState, convertsCalibratingState)
{
  std::string convertedState
    = telemetry::Writer::convertStateMachineState(data::State::kCalibrating);
  ASSERT_EQ(convertedState, "CALIBRATING") << "Should convert Calibrating state.";
}

TEST_F(WriterConvertStateMachineState, convertsPreCalibratingState)
{
  std::string convertedState
    = telemetry::Writer::convertStateMachineState(data::State::kPreCalibrating);
  ASSERT_EQ(convertedState, "PRE_CALIBRATING") << "Should convert Pre-calibrating state.";
}

TEST_F(WriterConvertStateMachineState, convertsReadyState)
{
  std::string convertedState = telemetry::Writer::convertStateMachineState(data::State::kReady);
  ASSERT_EQ(convertedState, "READY") << "Should convert Ready state.";
}

TEST_F(WriterConvertStateMachineState, convertsAcceleratingState)
{
  std::string convertedState
    = telemetry::Writer::convertStateMachineState(data::State::kAccelerating);
  ASSERT_EQ(convertedState, "ACCELERATING") << "Should convert Accelerating state.";
}

TEST_F(WriterConvertStateMachineState, convertsCruisingState)
{
  std::string convertedState = telemetry::Writer::convertStateMachineState(data::State::kCruising);
  ASSERT_EQ(convertedState, "CRUISING") << "Should convert Cruising state.";
}

TEST_F(WriterConvertStateMachineState, convertsNominalBrakingState)
{
  std::string convertedState
    = telemetry::Writer::convertStateMachineState(data::State::kNominalBraking);
  ASSERT_EQ(convertedState, "NOMINAL_BRAKING") << "Should convert Nominal Braking state.";
}

TEST_F(WriterConvertStateMachineState, convertsEmergencyBrakingState)
{
  std::string convertedState
    = telemetry::Writer::convertStateMachineState(data::State::kEmergencyBraking);
  ASSERT_EQ(convertedState, "EMERGENCY_BRAKING") << "Should convert Emergency Braking state.";
}

TEST_F(WriterConvertStateMachineState, convertsFailureStoppedState)
{
  std::string convertedState
    = telemetry::Writer::convertStateMachineState(data::State::kFailureStopped);
  ASSERT_EQ(convertedState, "FAILURE_STOPPED") << "Should convert Failure Stopped state.";
}

TEST_F(WriterConvertStateMachineState, convertsFinishedState)
{
  std::string convertedState = telemetry::Writer::convertStateMachineState(data::State::kFinished);
  ASSERT_EQ(convertedState, "FINISHED") << "Should convert Finished state.";
}

TEST_F(WriterConvertStateMachineState, convertsInvalidState)
{
  std::string convertedState = telemetry::Writer::convertStateMachineState(data::State::kInvalid);
  ASSERT_EQ(convertedState, "INVALID") << "Should convert Invalid state.";
}

}  // namespace hyped::testing
