#include <string>

#include "data/data.hpp"
#include "gtest/gtest.h"
#include "telemetry/writer.hpp"

using namespace hyped::telemetry;
using namespace hyped::data;

/**
 * Tests the conversion of State from Enum to String as required for GUI
 */
struct WriterConvertStateMachineState : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(WriterConvertStateMachineState, convertsIdleState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kIdle);
  ASSERT_EQ(convertedState, "IDLE") << "Should convert Idle state.";
}

TEST_F(WriterConvertStateMachineState, convertsCalibratingState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kCalibrating);
  ASSERT_EQ(convertedState, "CALIBRATING") << "Should convert Calibrating state.";
}

TEST_F(WriterConvertStateMachineState, convertsPreCalibratingState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kPreCalibrating);
  ASSERT_EQ(convertedState, "PRE_CALIBRATING") << "Should convert Pre-calibrating state.";
}

TEST_F(WriterConvertStateMachineState, convertsReadyState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kReady);
  ASSERT_EQ(convertedState, "READY") << "Should convert Ready state.";
}

TEST_F(WriterConvertStateMachineState, convertsAcceleratingState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kAccelerating);
  ASSERT_EQ(convertedState, "ACCELERATING") << "Should convert Accelerating state.";
}

TEST_F(WriterConvertStateMachineState, convertsCruisingState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kCruising);
  ASSERT_EQ(convertedState, "CRUISING") << "Should convert Cruising state.";
}

TEST_F(WriterConvertStateMachineState, convertsNominalBrakingState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kNominalBraking);
  ASSERT_EQ(convertedState, "NOMINAL_BRAKING") << "Should convert Nominal Braking state.";
}

TEST_F(WriterConvertStateMachineState, convertsEmergencyBrakingState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kEmergencyBraking);
  ASSERT_EQ(convertedState, "EMERGENCY_BRAKING") << "Should convert Emergency Braking state.";
}

TEST_F(WriterConvertStateMachineState, convertsFailureStoppedState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kFailureStopped);
  ASSERT_EQ(convertedState, "FAILURE_STOPPED") << "Should convert Failure Stopped state.";
}

TEST_F(WriterConvertStateMachineState, convertsFinishedState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kFinished);
  ASSERT_EQ(convertedState, "FINISHED") << "Should convert Finished state.";
}

TEST_F(WriterConvertStateMachineState, convertsInvalidState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kInvalid);
  ASSERT_EQ(convertedState, "INVALID") << "Should convert Invalid state.";
}
