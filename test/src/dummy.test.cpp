#include "gtest/gtest.h"
#include "utils/logger.hpp"
#include "data/data.hpp"
#include "state_machine/hyped-machine.hpp"

  using hyped::data::Data;
  void t() {
    Data& d = Data::getInstance();
  }
  TEST(dummy_test, f) {
    //Data &d = Data::getInstance();
    ASSERT_EQ(2,1+1);
  }
  /* struct stateMachineTest : public ::testing::Test {
    protected:
      utils::Logger _log;
      data::StateMachine _sm;
      Data *_d;
      void SetUp() {
        _d = &Data::getInstance(); 
        _sm = _d->getStateMachineData();
      }
      void TearDown() {delete &_sm;}
  };
  TEST_F(stateMachineTest, state_machine_init) {
    _sm.current_state = data::State::kAccelerating;
    _d->setStateMachineData(_sm);
    ASSERT_EQ(_d->getStateMachineData().current_state, data::State::kAccelerating);

  } */
