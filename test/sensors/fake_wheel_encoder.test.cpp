#include "test.hpp"

#include <gtest/gtest.h>
#include <sensors/fake_wheel_encoder.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::testing {

class FakeWheelEncoderTest : public Test {
 public:
  static constexpr uint64_t kSleepMillis = 100;
  static constexpr size_t kNumIterations = 10;
};

TEST_F(FakeWheelEncoderTest, nonDecreasingData)
{
  auto &data = data::Data::getInstance();
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kIdle;
    data.setStateMachineData(state_machine_data);
  }
  auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  sensors::FakeWheelEncoder fake_wheel_encoder(fake_trajectory, 0.1);
  data::CounterData previous_count = fake_wheel_encoder.getData();
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const auto current_counter_data = fake_wheel_encoder.getData();
    enableOutput();
    ASSERT_GE(current_counter_data.value, previous_count.value);
    disableOutput();
    previous_count = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kCruising;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const auto current_counter_data = fake_wheel_encoder.getData();
    enableOutput();
    ASSERT_GE(current_counter_data.value, previous_count.value);
    disableOutput();
    previous_count = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kNominalBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const auto current_counter_data = fake_wheel_encoder.getData();
    enableOutput();
    ASSERT_GE(current_counter_data.value, previous_count.value);
    disableOutput();
    previous_count = current_counter_data;
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kEmergencyBraking;
    data.setStateMachineData(state_machine_data);
  }
  for (size_t i = 0; i < kNumIterations; ++i) {
    utils::concurrent::Thread::sleep(kSleepMillis);
    const auto current_counter_data = fake_wheel_encoder.getData();
    enableOutput();
    ASSERT_GE(current_counter_data.value, previous_count.value);
    disableOutput();
    previous_count = current_counter_data;
  }
}

}  // namespace hyped::testing
