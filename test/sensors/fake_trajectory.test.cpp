#include "test.hpp"

#include <gtest/gtest.h>
#include <sensors/fake_trajectory.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::testing {

class FakeTrajectoryTest : public Test {
 public:
  const std::vector<data::State> pre_run_states_
    = {data::State::kIdle, data::State::kPreCalibrating, data::State::kCalibrating,
       data::State::kReady};
  const std::vector<data::State> braking_states_
    = {data::State::kCruising, data::State::kNominalBraking, data::State::kEmergencyBraking};
  const std::vector<data::State> post_run_states_
    = {data::State::kFailureStopped, data::State::kFinished};
};

TEST_F(FakeTrajectoryTest, parsesConfig)
{
  const auto fake_trajectory = sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath);
  enableOutput();
  ASSERT_TRUE(fake_trajectory);
  disableOutput();
  const auto &config = fake_trajectory->getConfig();
  enableOutput();
  ASSERT_FLOAT_EQ(1000.0, config.maximum_acceleration);
  ASSERT_FLOAT_EQ(2000.0, config.braking_deceleration);
  ASSERT_FLOAT_EQ(0.01, config.cruising_deceleration);
  disableOutput();
}

TEST_F(FakeTrajectoryTest, noMovementWithZeroAcceleration)
{
  auto fake_trajectory = sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath);
  auto &data           = data::Data::getInstance();
  for (const auto state : pre_run_states_) {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = state;
    data.setStateMachineData(state_machine_data);
    const auto trajectory = fake_trajectory->getTrajectory();
    enableOutput();
    ASSERT_FLOAT_EQ(0.0, trajectory.acceleration);
    ASSERT_FLOAT_EQ(0.0, trajectory.velocity);
    ASSERT_FLOAT_EQ(0.0, trajectory.displacement);
    disableOutput();
  }
}

TEST_F(FakeTrajectoryTest, expectedVelocities)
{
  auto fake_trajectory     = sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath);
  auto &data               = data::Data::getInstance();
  auto previous_trajectory = fake_trajectory->getTrajectory();
  // Pre-run
  {
    for (const auto state : pre_run_states_) {
      auto state_machine_data          = data.getStateMachineData();
      state_machine_data.current_state = state;
      data.setStateMachineData(state_machine_data);
      const auto trajectory = fake_trajectory->getTrajectory();
      enableOutput();
      ASSERT_EQ(trajectory.velocity, 0.0);
      disableOutput();
      previous_trajectory = trajectory;
    }
  }
  // Accelerating
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    const auto trajectory = fake_trajectory->getTrajectory();
    ASSERT_GT(trajectory.velocity, previous_trajectory.velocity);
    disableOutput();
    previous_trajectory = trajectory;
  }
  // Braking
  {
    for (const auto state : braking_states_) {
      auto state_machine_data          = data.getStateMachineData();
      state_machine_data.current_state = state;
      data.setStateMachineData(state_machine_data);
      const auto trajectory = fake_trajectory->getTrajectory();
      enableOutput();
      ASSERT_LT(trajectory.velocity, previous_trajectory.velocity);
      disableOutput();
      previous_trajectory = trajectory;
    }
  }
  // Post-run
  {
    for (const auto state : post_run_states_) {
      auto state_machine_data          = data.getStateMachineData();
      state_machine_data.current_state = state;
      data.setStateMachineData(state_machine_data);
      const auto trajectory = fake_trajectory->getTrajectory();
      enableOutput();
      ASSERT_FLOAT_EQ(trajectory.velocity, 0.0);
      disableOutput();
    }
  }
}

TEST_F(FakeTrajectoryTest, expectedAcclerations)
{
  auto fake_trajectory = sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath);
  auto &data           = data::Data::getInstance();
  // Pre-run
  {
    for (const auto state : pre_run_states_) {
      auto state_machine_data          = data.getStateMachineData();
      state_machine_data.current_state = state;
      data.setStateMachineData(state_machine_data);
      const auto trajectory = fake_trajectory->getTrajectory();
      enableOutput();
      ASSERT_FLOAT_EQ(trajectory.acceleration, 0.0);
      disableOutput();
    }
  }
  // Accelerating
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
    const auto trajectory = fake_trajectory->getTrajectory();
    enableOutput();
    ASSERT_GT(trajectory.acceleration, 0.0);
    disableOutput();
  }
  // Braking
  {
    for (const auto state : braking_states_) {
      auto state_machine_data          = data.getStateMachineData();
      state_machine_data.current_state = state;
      data.setStateMachineData(state_machine_data);
      const auto trajectory = fake_trajectory->getTrajectory();
      enableOutput();
      ASSERT_LT(trajectory.acceleration, 0.0);
      disableOutput();
    }
  }
  // Post-run
  {
    for (const auto state : post_run_states_) {
      auto state_machine_data          = data.getStateMachineData();
      state_machine_data.current_state = state;
      data.setStateMachineData(state_machine_data);
      const auto trajectory = fake_trajectory->getTrajectory();
      enableOutput();
      ASSERT_FLOAT_EQ(trajectory.acceleration, 0.0);
      disableOutput();
    }
  }
}

}  // namespace hyped::testing
