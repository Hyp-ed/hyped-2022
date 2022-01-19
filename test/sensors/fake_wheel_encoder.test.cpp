#include "test.hpp"

#include <gtest/gtest.h>
#include <sensors/fake_wheel_encoder.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::testing {

class FakeWheelEncoderTest : public Test {
 public:
  static constexpr uint64_t kSleepMillis = 100;
  static constexpr size_t kNumIterations = 10;
  const std::string kOneFaultyConfigPath = "configurations/test/fake_wheel_encoder_one_faulty.json";
  const std::string kFourFaultyConfigPath
    = "configurations/test/fake_wheel_encoder_four_faulty.json";
};

//---------------------------------------------------------------------------
// All operational
//---------------------------------------------------------------------------

TEST_F(FakeWheelEncoderTest, defaultParsesConfig)
{
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  const auto fake_wheel_encoders_optional
    = sensors::FakeWheelEncoder::fromFile(log_, kDefaultConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_wheel_encoders_optional);
  const auto fake_wheel_encoders = *fake_wheel_encoders_optional;
  ASSERT_EQ(data::Sensors::kNumEncoders, fake_wheel_encoders.size());
  ASSERT_FLOAT_EQ(0.1, fake_wheel_encoders.at(0).getConfig().noise);
  ASSERT_FLOAT_EQ(0.2, fake_wheel_encoders.at(1).getConfig().noise);
  ASSERT_FLOAT_EQ(0.1, fake_wheel_encoders.at(2).getConfig().noise);
  ASSERT_FLOAT_EQ(0.01, fake_wheel_encoders.at(3).getConfig().noise);
  ASSERT_FALSE(fake_wheel_encoders.at(0).getConfig().failure_in_state);
  ASSERT_FALSE(fake_wheel_encoders.at(1).getConfig().failure_in_state);
  ASSERT_FALSE(fake_wheel_encoders.at(2).getConfig().failure_in_state);
  ASSERT_FALSE(fake_wheel_encoders.at(3).getConfig().failure_in_state);
  disableOutput();
}
TEST_F(FakeWheelEncoderTest, defaultNonDecreasingData)
{
  auto &data = data::Data::getInstance();
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kIdle;
    data.setStateMachineData(state_machine_data);
  }
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  auto fake_wheel_encoders_optional
    = sensors::FakeWheelEncoder::fromFile(log_, kDefaultConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_wheel_encoders_optional);
  disableOutput();
  auto fake_wheel_encoders = *fake_wheel_encoders_optional;
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
  }
  std::array<data::CounterData, fake_wheel_encoders.size()> previous_counter_data = {
    fake_wheel_encoders.at(0).getData(),
    fake_wheel_encoders.at(1).getData(),
    fake_wheel_encoders.at(2).getData(),
    fake_wheel_encoders.at(3).getData(),
  };
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumEncoders> current_counter_data = {
      fake_wheel_encoders.at(0).getData(),
      fake_wheel_encoders.at(1).getData(),
      fake_wheel_encoders.at(2).getData(),
      fake_wheel_encoders.at(3).getData(),
    };
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_TRUE(current_counter_data.at(2).operational);
    ASSERT_TRUE(current_counter_data.at(3).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    ASSERT_GE(current_counter_data.at(2).value, previous_counter_data.at(2).value);
    ASSERT_GE(current_counter_data.at(3).value, previous_counter_data.at(3).value);
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
    const std::array<data::CounterData, data::Sensors::kNumEncoders> current_counter_data = {
      fake_wheel_encoders.at(0).getData(),
      fake_wheel_encoders.at(1).getData(),
      fake_wheel_encoders.at(2).getData(),
      fake_wheel_encoders.at(3).getData(),
    };
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_TRUE(current_counter_data.at(2).operational);
    ASSERT_TRUE(current_counter_data.at(3).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    ASSERT_GE(current_counter_data.at(2).value, previous_counter_data.at(2).value);
    ASSERT_GE(current_counter_data.at(3).value, previous_counter_data.at(3).value);
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
    const std::array<data::CounterData, data::Sensors::kNumEncoders> current_counter_data = {
      fake_wheel_encoders.at(0).getData(),
      fake_wheel_encoders.at(1).getData(),
      fake_wheel_encoders.at(2).getData(),
      fake_wheel_encoders.at(3).getData(),
    };
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_TRUE(current_counter_data.at(2).operational);
    ASSERT_TRUE(current_counter_data.at(3).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    ASSERT_GE(current_counter_data.at(2).value, previous_counter_data.at(2).value);
    ASSERT_GE(current_counter_data.at(3).value, previous_counter_data.at(3).value);
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
    const std::array<data::CounterData, data::Sensors::kNumEncoders> current_counter_data = {
      fake_wheel_encoders.at(0).getData(),
      fake_wheel_encoders.at(1).getData(),
      fake_wheel_encoders.at(2).getData(),
      fake_wheel_encoders.at(3).getData(),
    };
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_TRUE(current_counter_data.at(2).operational);
    ASSERT_TRUE(current_counter_data.at(3).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    ASSERT_GE(current_counter_data.at(2).value, previous_counter_data.at(2).value);
    ASSERT_GE(current_counter_data.at(3).value, previous_counter_data.at(3).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
}

//---------------------------------------------------------------------------
// One faulty, three operational
//---------------------------------------------------------------------------

TEST_F(FakeWheelEncoderTest, oneFaultyParsesConfig)
{
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  const auto fake_wheel_encoders_optional
    = sensors::FakeWheelEncoder::fromFile(log_, kOneFaultyConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_wheel_encoders_optional);
  const auto fake_wheel_encoders = *fake_wheel_encoders_optional;
  ASSERT_EQ(data::Sensors::kNumEncoders, fake_wheel_encoders.size());
  ASSERT_FLOAT_EQ(0.1, fake_wheel_encoders.at(0).getConfig().noise);
  ASSERT_FLOAT_EQ(0.2, fake_wheel_encoders.at(1).getConfig().noise);
  ASSERT_FLOAT_EQ(0.1, fake_wheel_encoders.at(2).getConfig().noise);
  ASSERT_FLOAT_EQ(0.01, fake_wheel_encoders.at(3).getConfig().noise);
  ASSERT_FALSE(fake_wheel_encoders.at(0).getConfig().failure_in_state);
  ASSERT_TRUE(fake_wheel_encoders.at(1).getConfig().failure_in_state);
  ASSERT_FALSE(fake_wheel_encoders.at(2).getConfig().failure_in_state);
  ASSERT_FALSE(fake_wheel_encoders.at(3).getConfig().failure_in_state);
  ASSERT_EQ(data::State::kNominalBraking, *fake_wheel_encoders.at(1).getConfig().failure_in_state);
  disableOutput();
}
TEST_F(FakeWheelEncoderTest, oneFaultyNonDecreasingData)
{
  auto &data = data::Data::getInstance();
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kIdle;
    data.setStateMachineData(state_machine_data);
  }
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  auto fake_wheel_encoders_optional
    = sensors::FakeWheelEncoder::fromFile(log_, kOneFaultyConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_wheel_encoders_optional);
  disableOutput();
  auto fake_wheel_encoders = *fake_wheel_encoders_optional;
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
  }
  std::array<data::CounterData, fake_wheel_encoders.size()> previous_counter_data = {
    fake_wheel_encoders.at(0).getData(),
    fake_wheel_encoders.at(1).getData(),
    fake_wheel_encoders.at(2).getData(),
    fake_wheel_encoders.at(3).getData(),
  };
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const std::array<data::CounterData, data::Sensors::kNumEncoders> current_counter_data = {
      fake_wheel_encoders.at(0).getData(),
      fake_wheel_encoders.at(1).getData(),
      fake_wheel_encoders.at(2).getData(),
      fake_wheel_encoders.at(3).getData(),
    };
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_TRUE(current_counter_data.at(2).operational);
    ASSERT_TRUE(current_counter_data.at(3).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    ASSERT_GE(current_counter_data.at(2).value, previous_counter_data.at(2).value);
    ASSERT_GE(current_counter_data.at(3).value, previous_counter_data.at(3).value);
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
    const std::array<data::CounterData, data::Sensors::kNumEncoders> current_counter_data = {
      fake_wheel_encoders.at(0).getData(),
      fake_wheel_encoders.at(1).getData(),
      fake_wheel_encoders.at(2).getData(),
      fake_wheel_encoders.at(3).getData(),
    };
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_TRUE(current_counter_data.at(1).operational);
    ASSERT_TRUE(current_counter_data.at(2).operational);
    ASSERT_TRUE(current_counter_data.at(3).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(1).value, previous_counter_data.at(1).value);
    ASSERT_GE(current_counter_data.at(2).value, previous_counter_data.at(2).value);
    ASSERT_GE(current_counter_data.at(3).value, previous_counter_data.at(3).value);
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
    const std::array<data::CounterData, data::Sensors::kNumEncoders> current_counter_data = {
      fake_wheel_encoders.at(0).getData(),
      fake_wheel_encoders.at(1).getData(),
      fake_wheel_encoders.at(2).getData(),
      fake_wheel_encoders.at(3).getData(),
    };
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_FALSE(current_counter_data.at(1).operational);
    ASSERT_TRUE(current_counter_data.at(2).operational);
    ASSERT_TRUE(current_counter_data.at(3).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(2).value, previous_counter_data.at(2).value);
    ASSERT_GE(current_counter_data.at(3).value, previous_counter_data.at(3).value);
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
    const std::array<data::CounterData, data::Sensors::kNumEncoders> current_counter_data = {
      fake_wheel_encoders.at(0).getData(),
      fake_wheel_encoders.at(1).getData(),
      fake_wheel_encoders.at(2).getData(),
      fake_wheel_encoders.at(3).getData(),
    };
    enableOutput();
    ASSERT_TRUE(current_counter_data.at(0).operational);
    ASSERT_FALSE(current_counter_data.at(1).operational);
    ASSERT_TRUE(current_counter_data.at(2).operational);
    ASSERT_TRUE(current_counter_data.at(3).operational);
    ASSERT_GE(current_counter_data.at(0).value, previous_counter_data.at(0).value);
    ASSERT_GE(current_counter_data.at(2).value, previous_counter_data.at(2).value);
    ASSERT_GE(current_counter_data.at(3).value, previous_counter_data.at(3).value);
    disableOutput();
    previous_counter_data = current_counter_data;
  }
}

}  // namespace hyped::testing
