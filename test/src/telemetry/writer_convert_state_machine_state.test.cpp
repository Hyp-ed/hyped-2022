/*
 * Author: Robertas Norkus
 * Organisation: HYPED
 * Date: 22/03/2021
 * Description: Tests the conversion of State from Enum to String as required for GUI
 *
 *    Copyright 2021 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

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
  // ---- Error messages -------
  const std::string idle_state_error              = "Should convert Idle state.";
  const std::string calibrating_state_error       = "Should convert Calibrating state.";
  const std::string ready_state_error             = "Should convert Ready state.";
  const std::string accelerating_state_error      = "Should convert Accelerating state.";
  const std::string cruising_state_error          = "Should convert Cruising state.";
  const std::string nominal_braking_state_error   = "Should convert Nominal Braking state.";
  const std::string emergency_braking_state_error = "Should convert Emergency Braking state.";
  const std::string failure_stopped_state_error   = "Should convert Failure Stopped state.";
  const std::string finished_state_error          = "Should convert Finished state.";
  const std::string invalid_state_error           = "Should convert Invalid state.";

 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(WriterConvertStateMachineState, convertsIdleState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kIdle);
  ASSERT_EQ(convertedState, "IDLE") << idle_state_error;
}

TEST_F(WriterConvertStateMachineState, convertsCalibratingState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kCalibrating);
  ASSERT_EQ(convertedState, "CALIBRATING") << calibrating_state_error;
}

TEST_F(WriterConvertStateMachineState, convertsReadyState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kReady);
  ASSERT_EQ(convertedState, "READY") << ready_state_error;
}

TEST_F(WriterConvertStateMachineState, convertsAcceleratingState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kAccelerating);
  ASSERT_EQ(convertedState, "ACCELERATING") << accelerating_state_error;
}

TEST_F(WriterConvertStateMachineState, convertsCruisingState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kCruising);
  ASSERT_EQ(convertedState, "CRUISING") << cruising_state_error;
}

TEST_F(WriterConvertStateMachineState, convertsNominalBrakingState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kNominalBraking);
  ASSERT_EQ(convertedState, "NOMINAL_BRAKING") << nominal_braking_state_error;
}

TEST_F(WriterConvertStateMachineState, convertsEmergencyBrakingState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kEmergencyBraking);
  ASSERT_EQ(convertedState, "EMERGENCY_BRAKING") << emergency_braking_state_error;
}

TEST_F(WriterConvertStateMachineState, convertsFailureStoppedState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kFailureStopped);
  ASSERT_EQ(convertedState, "FAILURE_STOPPED") << failure_stopped_state_error;
}

TEST_F(WriterConvertStateMachineState, convertsFinishedState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kFinished);
  ASSERT_EQ(convertedState, "FINISHED") << finished_state_error;
}

TEST_F(WriterConvertStateMachineState, convertsInvalidState)
{
  std::string convertedState = Writer::convertStateMachineState(State::kInvalid);
  ASSERT_EQ(convertedState, "INVALID") << invalid_state_error;
}
