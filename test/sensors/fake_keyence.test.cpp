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
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  disableOutput();
  auto fake_keyences = sensors::FakeKeyence::fromFile(log_, kDefaultConfigPath, fake_trajectory);
  for (auto fake_keyence : *fake_keyences) {
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
}

TEST_F(FakeKeyenceTest, parsesConfig)
{
  auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  const auto fake_keyences_optional
    = sensors::FakeKeyence::fromFile(log_, kDefaultConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_keyences_optional);
  const auto fake_keyences = *fake_keyences_optional;
  ASSERT_EQ(data::Sensors::kNumKeyence, fake_keyences.size());
  ASSERT_FLOAT_EQ(0.1, fake_keyences.at(0).getConfig().noise);
  ASSERT_EQ(data::State::kNominalBraking, *fake_keyences.at(0).getConfig().failure_in_state);
  ASSERT_FLOAT_EQ(0.2, fake_keyences.at(1).getConfig().noise);
  ASSERT_FALSE(fake_keyences.at(1).getConfig().failure_in_state);
  disableOutput();
}

}  // namespace hyped::testing
