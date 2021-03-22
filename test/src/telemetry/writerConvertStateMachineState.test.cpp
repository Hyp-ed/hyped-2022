/*
 * Author: Robertas Norkus
 * Organisation: HYPED
 * Date:
 * Description:
 *
 *    Copyright 2019 HYPED
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

#include "gtest/gtest.h"
#include "telemetry/writer.hpp"
#include "data/data.hpp"

using namespace hyped::telemetry;
using namespace hyped::data;

struct WriterConvertStateMachineState : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(WriterConvertStateMachineState, idleState)
{
  ASSERT_EQ(Writer::convertStateMachineState(State::kIdle), "IDLE");
}

TEST_F(WriterConvertStateMachineState, calibratingState)
{
  ASSERT_EQ(Writer::convertStateMachineState(State::kCalibrating), "CALIBRATING");
}

TEST_F(WriterConvertStateMachineState, readyState)
{
  ASSERT_EQ(Writer::convertStateMachineState(State::kReady), "READY");
}

TEST_F(WriterConvertStateMachineState, acceleratingState)
{
  ASSERT_EQ(Writer::convertStateMachineState(State::kAccelerating), "ACCELERATING");
}

TEST_F(WriterConvertStateMachineState, nominalBrakingState)
{
  ASSERT_EQ(Writer::convertStateMachineState(State::kNominalBraking), "NOMINAL_BRAKING");
}

TEST_F(WriterConvertStateMachineState, emergencyBrakingState)
{
  ASSERT_EQ(Writer::convertStateMachineState(State::kEmergencyBraking), "EMERGENCY_BRAKING");
}

TEST_F(WriterConvertStateMachineState, runCompleteState)
{
  ASSERT_EQ(Writer::convertStateMachineState(State::kRunComplete), "RUN_COMPLETE");
}

TEST_F(WriterConvertStateMachineState, failureStoppedState)
{
  ASSERT_EQ(Writer::convertStateMachineState(State::kFailureStopped), "FAILURE_STOPPED");
}

TEST_F(WriterConvertStateMachineState, exitingState)
{
  ASSERT_EQ(Writer::convertStateMachineState(State::kExiting), "EXITING");
}

TEST_F(WriterConvertStateMachineState, finishedState)
{
  ASSERT_EQ(Writer::convertStateMachineState(State::kFinished), "FINISHED");
}

TEST_F(WriterConvertStateMachineState, invalidState)
{
  ASSERT_EQ(Writer::convertStateMachineState(State::kInvalid), "INVALID");
}
