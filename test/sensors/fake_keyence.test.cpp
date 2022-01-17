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
  const std::string kTwoFaultyConfigPath = "configurations/test/fake_keyence_two_faulty.json";
};

//---------------------------------------------------------------------------
// Both operational
//---------------------------------------------------------------------------

TEST_F(FakeKeyenceTest, defaultParsesConfig)
{
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  const auto fake_keyences_optional
    = sensors::FakeKeyence::fromFile(log_, kDefaultConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_keyences_optional);
  const auto fake_keyences = *fake_keyences_optional;
  ASSERT_EQ(data::Sensors::kNumKeyence, fake_keyences.size());
  ASSERT_FLOAT_EQ(0.1, fake_keyences.at(0).getConfig().noise);
  ASSERT_FLOAT_EQ(0.2, fake_keyences.at(1).getConfig().noise);
  ASSERT_FALSE(fake_keyences.at(0).getConfig().failure_in_state);
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
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  auto fake_keyences_optional
    = sensors::FakeKeyence::fromFile(log_, kDefaultConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_keyences_optional);
  disableOutput();
  auto fake_keyences = *fake_keyences_optional;
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
  }
  std::array<data::CounterData, fake_keyences.size()> previous_counter_data
    = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kCruising;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kNominalBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kEmergencyBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
}

//---------------------------------------------------------------------------
// One faulty, one operational
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
  ASSERT_FLOAT_EQ(0.2, fake_keyences.at(1).getConfig().noise);
  ASSERT_TRUE(fake_keyences.at(0).getConfig().failure_in_state);
  ASSERT_FALSE(fake_keyences.at(1).getConfig().failure_in_state);
  ASSERT_EQ(data::State::kNominalBraking, *fake_keyences.at(0).getConfig().failure_in_state);
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
  std::array<data::CounterData, fake_keyences.size()> previous_counter_data
    = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kCruising;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kNominalBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_FALSE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kEmergencyBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_FALSE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
}

//---------------------------------------------------------------------------
// Two faulty
//---------------------------------------------------------------------------

TEST_F(FakeKeyenceTest, twoFaultyParsesConfig)
{
  auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  enableOutput();
  const auto fake_keyences_optional
    = sensors::FakeKeyence::fromFile(log_, kTwoFaultyConfigPath, fake_trajectory);
  ASSERT_TRUE(fake_keyences_optional);
  const auto fake_keyences = *fake_keyences_optional;
  ASSERT_EQ(data::Sensors::kNumKeyence, fake_keyences.size());
  ASSERT_FLOAT_EQ(0.1, fake_keyences.at(0).getConfig().noise);
  ASSERT_FLOAT_EQ(0.2, fake_keyences.at(1).getConfig().noise);
  ASSERT_TRUE(fake_keyences.at(0).getConfig().failure_in_state);
  ASSERT_TRUE(fake_keyences.at(1).getConfig().failure_in_state);
  ASSERT_EQ(data::State::kAccelerating, *fake_keyences.at(0).getConfig().failure_in_state);
  ASSERT_EQ(data::State::kNominalBraking, *fake_keyences.at(1).getConfig().failure_in_state);
  disableOutput();
}

TEST_F(FakeKeyenceTest, twoFaultyNonDecreasingData)
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
    = sensors::FakeKeyence::fromFile(log_, kTwoFaultyConfigPath, fake_trajectory);
  ASSERT_TRUE(fake_keyences_optional);
  auto fake_keyences = *fake_keyences_optional;
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
  }
  std::array<data::CounterData, fake_keyences.size()> previous_counter_data
    = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_FALSE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kCruising;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_FALSE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kNominalBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_FALSE(current_counter_data.at(0).operational);
    ASSERT_FALSE(current_counter_data.at(1).operational);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kEmergencyBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumKeyence> current_counter_data
      = {fake_keyences.at(0).getData(), fake_keyences.at(1).getData()};
    enableOutput();
    ASSERT_FALSE(current_counter_data.at(0).operational);
    ASSERT_FALSE(current_counter_data.at(1).operational);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
}

}  // namespace hyped::testing
