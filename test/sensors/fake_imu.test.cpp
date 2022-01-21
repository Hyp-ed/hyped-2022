#include "test.hpp"

#include <gtest/gtest.h>
#include <sensors/fake_imu.hpp>
#include <utils/concurrent/thread.hpp>

namespace hyped::testing {

class FakeImuTest : public Test {
 public:
  inline static const std::string kOneFaultyConfigPath
    = "configurations/test/fake_imu_one_faulty.json";
  inline static const std::string kFourFaultyConfigPath
    = "configurations/test/fake_imu_four_faulty.json";
};

//---------------------------------------------------------------------------
// All operational
//---------------------------------------------------------------------------

TEST_F(FakeImuTest, defaultParsesConfig)
{
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  const auto fake_imus_optional
    = sensors::FakeImu::fromFile(log_, kDefaultConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_imus_optional);
  const auto fake_imus = *fake_imus_optional;
  ASSERT_EQ(data::Sensors::kNumImus, fake_imus.size());
  ASSERT_FLOAT_EQ(0.01, fake_imus.at(0).getConfig().noise);
  ASSERT_FLOAT_EQ(0.02, fake_imus.at(1).getConfig().noise);
  ASSERT_FLOAT_EQ(0.005, fake_imus.at(2).getConfig().noise);
  ASSERT_FLOAT_EQ(0.001, fake_imus.at(3).getConfig().noise);
  ASSERT_FALSE(fake_imus.at(0).getConfig().failure_in_state);
  ASSERT_FALSE(fake_imus.at(1).getConfig().failure_in_state);
  ASSERT_FALSE(fake_imus.at(2).getConfig().failure_in_state);
  ASSERT_FALSE(fake_imus.at(3).getConfig().failure_in_state);
  disableOutput();
}

TEST_F(FakeImuTest, defaultAllOperational)
{
  auto &data                 = data::Data::getInstance();
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  auto fake_imus_optional = sensors::FakeImu::fromFile(log_, kDefaultConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_imus_optional);
  disableOutput();
  auto fake_imus = *fake_imus_optional;
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kReady;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_TRUE(fake_imus.at(2).getData().operational);
    ASSERT_TRUE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_TRUE(fake_imus.at(2).getData().operational);
    ASSERT_TRUE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kCruising;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_TRUE(fake_imus.at(2).getData().operational);
    ASSERT_TRUE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kNominalBraking;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_TRUE(fake_imus.at(2).getData().operational);
    ASSERT_TRUE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kEmergencyBraking;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_TRUE(fake_imus.at(2).getData().operational);
    ASSERT_TRUE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
}

//---------------------------------------------------------------------------
// One faulty, three operational
//---------------------------------------------------------------------------

TEST_F(FakeImuTest, oneFaultyParsesConfig)
{
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  const auto fake_imus_optional
    = sensors::FakeImu::fromFile(log_, kOneFaultyConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_imus_optional);
  const auto fake_imus = *fake_imus_optional;
  ASSERT_EQ(data::Sensors::kNumImus, fake_imus.size());
  ASSERT_FLOAT_EQ(0.02, fake_imus.at(0).getConfig().noise);
  ASSERT_FLOAT_EQ(0.001, fake_imus.at(1).getConfig().noise);
  ASSERT_FLOAT_EQ(0.01, fake_imus.at(2).getConfig().noise);
  ASSERT_FLOAT_EQ(0.005, fake_imus.at(3).getConfig().noise);
  ASSERT_FALSE(fake_imus.at(0).getConfig().failure_in_state);
  ASSERT_FALSE(fake_imus.at(1).getConfig().failure_in_state);
  ASSERT_TRUE(fake_imus.at(2).getConfig().failure_in_state);
  ASSERT_FALSE(fake_imus.at(3).getConfig().failure_in_state);
  ASSERT_EQ(data::State::kCruising, fake_imus.at(2).getConfig().failure_in_state);
  disableOutput();
}

TEST_F(FakeImuTest, oneFaultyExpectedFailure)
{
  auto &data                 = data::Data::getInstance();
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  auto fake_imus_optional = sensors::FakeImu::fromFile(log_, kOneFaultyConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_imus_optional);
  disableOutput();
  auto fake_imus = *fake_imus_optional;
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kReady;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_TRUE(fake_imus.at(2).getData().operational);
    ASSERT_TRUE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_TRUE(fake_imus.at(2).getData().operational);
    ASSERT_TRUE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kCruising;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_FALSE(fake_imus.at(2).getData().operational);
    ASSERT_TRUE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kNominalBraking;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_FALSE(fake_imus.at(2).getData().operational);
    ASSERT_TRUE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kEmergencyBraking;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_FALSE(fake_imus.at(2).getData().operational);
    ASSERT_TRUE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
}

//---------------------------------------------------------------------------
// Four faulty
//---------------------------------------------------------------------------

TEST_F(FakeImuTest, fourFaultyParsesConfig)
{
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  const auto fake_imus_optional
    = sensors::FakeImu::fromFile(log_, kFourFaultyConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_imus_optional);
  const auto fake_imus = *fake_imus_optional;
  ASSERT_EQ(data::Sensors::kNumImus, fake_imus.size());
  ASSERT_FLOAT_EQ(0.04, fake_imus.at(0).getConfig().noise);
  ASSERT_FLOAT_EQ(0.021, fake_imus.at(1).getConfig().noise);
  ASSERT_FLOAT_EQ(0.03, fake_imus.at(2).getConfig().noise);
  ASSERT_FLOAT_EQ(0.09, fake_imus.at(3).getConfig().noise);
  ASSERT_TRUE(fake_imus.at(0).getConfig().failure_in_state);
  ASSERT_TRUE(fake_imus.at(1).getConfig().failure_in_state);
  ASSERT_TRUE(fake_imus.at(2).getConfig().failure_in_state);
  ASSERT_TRUE(fake_imus.at(3).getConfig().failure_in_state);
  ASSERT_EQ(data::State::kNominalBraking, fake_imus.at(0).getConfig().failure_in_state);
  ASSERT_EQ(data::State::kCruising, fake_imus.at(1).getConfig().failure_in_state);
  ASSERT_EQ(data::State::kEmergencyBraking, fake_imus.at(2).getConfig().failure_in_state);
  ASSERT_EQ(data::State::kAccelerating, fake_imus.at(3).getConfig().failure_in_state);
  disableOutput();
}

TEST_F(FakeImuTest, fourFaultyExpectedFailure)
{
  auto &data                 = data::Data::getInstance();
  const auto fake_trajectory = std::make_shared<sensors::FakeTrajectory>(
    *sensors::FakeTrajectory::fromFile(log_, kDefaultConfigPath));
  auto fake_imus_optional
    = sensors::FakeImu::fromFile(log_, kFourFaultyConfigPath, fake_trajectory);
  enableOutput();
  ASSERT_TRUE(fake_imus_optional);
  disableOutput();
  auto fake_imus = *fake_imus_optional;
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kReady;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_TRUE(fake_imus.at(2).getData().operational);
    ASSERT_TRUE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kAccelerating;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_TRUE(fake_imus.at(1).getData().operational);
    ASSERT_TRUE(fake_imus.at(2).getData().operational);
    ASSERT_FALSE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kCruising;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_TRUE(fake_imus.at(0).getData().operational);
    ASSERT_FALSE(fake_imus.at(1).getData().operational);
    ASSERT_TRUE(fake_imus.at(2).getData().operational);
    ASSERT_FALSE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kNominalBraking;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_FALSE(fake_imus.at(0).getData().operational);
    ASSERT_FALSE(fake_imus.at(1).getData().operational);
    ASSERT_TRUE(fake_imus.at(2).getData().operational);
    ASSERT_FALSE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
  {
    auto state_machine_data          = data.getStateMachineData();
    state_machine_data.current_state = data::State::kEmergencyBraking;
    data.setStateMachineData(state_machine_data);
    enableOutput();
    ASSERT_FALSE(fake_imus.at(0).getData().operational);
    ASSERT_FALSE(fake_imus.at(1).getData().operational);
    ASSERT_FALSE(fake_imus.at(2).getData().operational);
    ASSERT_FALSE(fake_imus.at(3).getData().operational);
    disableOutput();
  }
}
}  // namespace hyped::testing
