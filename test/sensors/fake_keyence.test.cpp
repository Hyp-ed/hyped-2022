#include "test.hpp"

#include <gtest/gtest.h>
#include <sensors/fake_keyence.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::testing {

class FakeKeyenceTest : public Test {
 public:
  static constexpr uint64_t kSleepMillis = 100;
  static constexpr size_t kNumIterations = 10;
};

TEST_F(FakeKeyenceTest, nonDecreasingData)
{
  auto &data = data::Data::getInstance();
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kIdle;
    data.setStateMachineData(state_machine_data);
  }
  enableOutput();
  auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  disableOutput();
  sensors::FakeKeyence fake_keyence(fake_trajectory, {std::nullopt, 0.1});
  data::CounterData previous_count = fake_keyence.getData();
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const auto current_count = fake_keyence.getData();
    enableOutput();
    ASSERT_GE(current_count.value, previous_count.value);
    disableOutput();
    previous_count = current_count;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kCruising;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const auto current_count = fake_keyence.getData();
    enableOutput();
    ASSERT_GE(current_count.value, previous_count.value);
    disableOutput();
    previous_count = current_count;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kNominalBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const auto current_count = fake_keyence.getData();
    enableOutput();
    ASSERT_GE(current_count.value, previous_count.value);
    disableOutput();
    previous_count = current_count;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kEmergencyBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const auto current_count = fake_keyence.getData();
    enableOutput();
    ASSERT_GE(current_count.value, previous_count.value);
    disableOutput();
    previous_count = current_count;
  }
}

}  // namespace hyped::testing
