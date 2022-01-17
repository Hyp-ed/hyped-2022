#include "test.hpp"

#include <gtest/gtest.h>
#include <sensors/fake_keyence.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::testing {

class FakeKeyenceTest : public Test {
 public:
  static constexpr uint64_t kSleepMillis = 100;
  static constexpr size_t kNumIterations = 10;
  const std::string kOneFaultyConfigPath = "configurations/test/fake_keyence_one_faulty.json";
};

//---------------------------------------------------------------------------
// Default configuration (no failures, all operational)
//---------------------------------------------------------------------------

TEST_F(FakeKeyenceTest, defaultParsesConfig)
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
  ASSERT_FALSE(fake_keyences.at(0).getConfig().failure_in_state);
  ASSERT_FLOAT_EQ(0.2, fake_keyences.at(1).getConfig().noise);
  ASSERT_FALSE(fake_keyences.at(1).getConfig().failure_in_state);
  disableOutput();
}

TEST_F(FakeKeyenceTest, defaultNonDecreasingData)
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
  auto fake_keyences_optional
    = sensors::FakeKeyence::fromFile(log_, kDefaultConfigPath, fake_trajectory);
  ASSERT_TRUE(fake_keyences_optional);
  auto fake_keyences = *fake_keyences_optional;
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
  }
  std::array<data::CounterData, fake_keyences.size()> previous_counter_data;
  for (size_t i = 0; i < fake_keyences.size(); ++i) {
    previous_counter_data.at(i) = fake_keyences.at(i).getData();
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    for (size_t i = 0; i < fake_keyences.size(); ++i) {
      const auto current_counter_data = fake_keyences.at(i).getData();
      enableOutput();
      ASSERT_TRUE(current_counter_data.operational);
      ASSERT_GE(current_counter_data.value, previous_counter_data.at(i).value);
      disableOutput();
      previous_counter_data.at(i) = current_counter_data;
    }
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kCruising;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    for (size_t i = 0; i < fake_keyences.size(); ++i) {
      const auto current_counter_data = fake_keyences.at(i).getData();
      enableOutput();
      ASSERT_TRUE(current_counter_data.operational);
      ASSERT_GE(current_counter_data.value, previous_counter_data.at(i).value);
      disableOutput();
      previous_counter_data.at(i) = current_counter_data;
    }
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kNominalBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    for (size_t i = 0; i < fake_keyences.size(); ++i) {
      const auto current_counter_data = fake_keyences.at(i).getData();
      enableOutput();
      ASSERT_TRUE(current_counter_data.operational);
      ASSERT_GE(current_counter_data.value, previous_counter_data.at(i).value);
      disableOutput();
      previous_counter_data.at(i) = current_counter_data;
    }
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kEmergencyBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    for (size_t i = 0; i < fake_keyences.size(); ++i) {
      const auto current_counter_data = fake_keyences.at(i).getData();
      enableOutput();
      ASSERT_TRUE(current_counter_data.operational);
      ASSERT_GE(current_counter_data.value, previous_counter_data.at(i).value);
      disableOutput();
      previous_counter_data.at(i) = current_counter_data;
    }
  }
}

//---------------------------------------------------------------------------
// One failure
//---------------------------------------------------------------------------

TEST_F(FakeKeyenceTest, oneFaultyParsesConfig)
{
  auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  const auto fake_keyences_optional
    = sensors::FakeKeyence::fromFile(log_, kOneFaultyConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_keyences_optional);
  const auto fake_keyences = *fake_keyences_optional;
  ASSERT_EQ(data::Sensors::kNumKeyence, fake_keyences.size());
  ASSERT_FLOAT_EQ(0.1, fake_keyences.at(0).getConfig().noise);
  ASSERT_TRUE(fake_keyences.at(0).getConfig().failure_in_state);
  ASSERT_EQ(data::State::kNominalBraking, *fake_keyences.at(0).getConfig().failure_in_state);
  ASSERT_FLOAT_EQ(0.2, fake_keyences.at(1).getConfig().noise);
  ASSERT_FALSE(fake_keyences.at(1).getConfig().failure_in_state);
  disableOutput();
}
TEST_F(FakeKeyenceTest, oneFaultyNonDecreasingData)
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
  auto fake_keyences_optional
    = sensors::FakeKeyence::fromFile(log_, kOneFaultyConfigPath, fake_trajectory);
  ASSERT_TRUE(fake_keyences_optional);
  auto fake_keyences = *fake_keyences_optional;
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
  }
  std::array<data::CounterData, fake_keyences.size()> previous_counter_data;
  for (size_t i = 0; i < fake_keyences.size(); ++i) {
    previous_counter_data.at(i) = fake_keyences.at(i).getData();
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    for (size_t i = 0; i < fake_keyences.size(); ++i) {
      const auto current_counter_data = fake_keyences.at(i).getData();
      enableOutput();
      if (current_counter_data.operational) {
        ASSERT_GE(current_counter_data.value, previous_counter_data.at(i).value);
      }
      disableOutput();
      previous_counter_data.at(i) = current_counter_data;
    }
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kCruising;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    for (size_t i = 0; i < fake_keyences.size(); ++i) {
      const auto current_counter_data = fake_keyences.at(i).getData();
      enableOutput();
      if (current_counter_data.operational) {
        ASSERT_GE(current_counter_data.value, previous_counter_data.at(i).value);
      }
      disableOutput();
      previous_counter_data.at(i) = current_counter_data;
    }
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kNominalBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    for (size_t i = 0; i < fake_keyences.size(); ++i) {
      const auto current_counter_data = fake_keyences.at(i).getData();
      if (current_counter_data.operational) {
        enableOutput();
        ASSERT_GE(current_counter_data.value, previous_counter_data.at(i).value);
        disableOutput();
      }
      previous_counter_data.at(i) = current_counter_data;
    }
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kEmergencyBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    for (size_t i = 0; i < fake_keyences.size(); ++i) {
      const auto current_counter_data = fake_keyences.at(i).getData();
      enableOutput();
      if (current_counter_data.operational) {
        ASSERT_GE(current_counter_data.value, previous_counter_data.at(i).value);
      }
      disableOutput();
      previous_counter_data.at(i) = current_counter_data;
    }
  }
}

}  // namespace hyped::testing
