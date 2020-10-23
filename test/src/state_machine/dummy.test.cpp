/*
* Author: QA team
* Organisation: HYPED
* Date:
* Description:
*
*    Copyright 2019 HYPED
*    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
*    except in compliance with the License. You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
*    Unless required by applicable law or agreed to in writing, software distributed under
*    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
*    either express or implied. See the License for the specific language governing permissions and
*    limitations under the License.
*/

#include "gtest/gtest.h"
#include "utils/logger.hpp"
#include "data/data.hpp"

using hyped::data::Data;
void t()
{
  // Data& d = Data::getInstance();
}

TEST(dummy_test, f)
{
  // Data &d = Data::getInstance();
  ASSERT_EQ(2, 1+1);
  // ASSERT_EQ(3,1+1);
}

struct stateMachineTest : public ::testing::Test {
 protected:
  hyped::utils::Logger _log;
  hyped::data::StateMachine _sm;
  Data *_d;
  void SetUp()
  {
    _d = &Data::getInstance();
    _sm = _d->getStateMachineData();
  }
  void TearDown() {}
};

// Test fixture for 'accelerating' state
TEST_F(stateMachineTest, state_machine_init)
{
  _sm.current_state = hyped::data::State::kAccelerating;
  _d->setStateMachineData(_sm);
  ASSERT_EQ(_d->getStateMachineData().current_state, hyped::data::State::kAccelerating);
}
