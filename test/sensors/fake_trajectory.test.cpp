#include "test.hpp"

#include <gtest/gtest.h>

#include <sensors/fake_trajectory.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::testing {

class FakeTrajectoryTest : public Test {
 public:
  inline static const std::vector<data::State> pre_run_states_
    = {data::State::kIdle, data::State::kPreCalibrating, data::State::kCalibrating,
       data::State::kReady};
  inline static const std::vector<data::State> braking_states_
    = {data::State::kCruising, data::State::kNominalBraking, data::State::kEmergencyBraking};
  inline static const std::vector<data::State> post_run_states_
    = {data::State::kFailureStopped, data::State::kFinished};
};

TEST_F(FakeTrajectoryTest, parsesConfig)
{
  const auto fake_trajectory = sensors::FakeTrajectory::fromFile(kDefaultConfigPath);
  ASSERT_TRUE(fake_trajectory);
  const auto &config = fake_trajectory->getConfig();
  ASSERT_FLOAT_EQ(1000.0, config.maximum_acceleration);
  ASSERT_FLOAT_EQ(2000.0, config.braking_deceleration);
  ASSERT_FLOAT_EQ(0.01, config.cruising_deceleration);
}

TEST_F(FakeTrajectoryTest, noMovementWithZeroAcceleration)
{
  auto fake_trajectory = sensors::FakeTrajectory::fromFile(kDefaultConfigPath);
  auto &data           = data::Data::getInstance();
  for (const auto state : pre_run_states_) {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = state;
    data.setStateMachineData(state_machine_data);
    const auto trajectory = fake_trajectory->getTrajectory();
    ASSERT_FLOAT_EQ(0.0, trajectory.acceleration);
    ASSERT_FLOAT_EQ(0.0, trajectory.velocity);
    ASSERT_FLOAT_EQ(0.0, trajectory.displacement);
  }
}

TEST_F(FakeTrajectoryTest, expectedVelocities)
{
  auto fake_trajectory     = sensors::FakeTrajectory::fromFile(kDefaultConfigPath);
  auto &data               = data::Data::getInstance();
  auto previous_trajectory = fake_trajectory->getTrajectory();
  // Pre-run
  {
    for (const auto state : pre_run_states_) {
      auto state_machine_data          = data.getStateMachineData();
      state_machine_data.current_state = state;
      data.setStateMachineData(state_machine_data);
      const auto trajectory = fake_trajectory->getTrajectory();
      ASSERT_EQ(trajectory.velocity, 0.0);
      previous_trajectory = trajectory;
    }
  }
  // Accelerating
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
    const auto trajectory = fake_trajectory->getTrajectory();
    ASSERT_GE(trajectory.velocity, previous_trajectory.velocity);
    previous_trajectory = trajectory;
  }
  // Braking
  {
    for (const auto state : braking_states_) {
      auto state_machine_data          = data.getStateMachineData();
      state_machine_data.current_state = state;
      data.setStateMachineData(state_machine_data);
      const auto trajectory = fake_trajectory->getTrajectory();
      ASSERT_LE(trajectory.velocity, previous_trajectory.velocity);
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
      ASSERT_FLOAT_EQ(trajectory.velocity, 0.0);
    }
  }
}

TEST_F(FakeTrajectoryTest, expectedAcclerations)
{
  auto fake_trajectory = sensors::FakeTrajectory::fromFile(kDefaultConfigPath);
  auto &data           = data::Data::getInstance();
  // Pre-run
  {
    for (const auto state : pre_run_states_) {
      auto state_machine_data          = data.getStateMachineData();
      state_machine_data.current_state = state;
      data.setStateMachineData(state_machine_data);
      const auto trajectory = fake_trajectory->getTrajectory();
      ASSERT_FLOAT_EQ(trajectory.acceleration, 0.0);
    }
  }
  // Accelerating
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
    const auto trajectory = fake_trajectory->getTrajectory();
    ASSERT_GT(trajectory.acceleration, 0.0);
  }
  // Braking
  {
    for (const auto state : braking_states_) {
      auto state_machine_data          = data.getStateMachineData();
      state_machine_data.current_state = state;
      data.setStateMachineData(state_machine_data);
      const auto trajectory = fake_trajectory->getTrajectory();
      ASSERT_LT(trajectory.acceleration, 0.0);
    }
  }
  // Post-run
  {
    for (const auto state : post_run_states_) {
      auto state_machine_data          = data.getStateMachineData();
      state_machine_data.current_state = state;
      data.setStateMachineData(state_machine_data);
      const auto trajectory = fake_trajectory->getTrajectory();
      ASSERT_FLOAT_EQ(trajectory.acceleration, 0.0);
    }
  }
}

}  // namespace hyped::testing
