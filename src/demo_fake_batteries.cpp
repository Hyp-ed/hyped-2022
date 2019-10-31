#include "utils/concurrent/thread.hpp"
#include "sensors/fake_batteries.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"
#include "utils/system.hpp"
#include "data/data.hpp"

using hyped::utils::concurrent::Thread;
using hyped::sensors::FakeBatteries;
using hyped::utils::Logger;
using hyped::utils::System;
using hyped::data::Data;
using hyped::data::State;
using hyped::BatteryData;

uint8_t kStripeNum = 30;

int main(int argc, char* argv[]) {
  hyped::utils::System::parseArgs(argc, argv);
  Logger log = System::getLogger();
  System& sys_ = System::getSystem();     // use --fake_batteries_fail flag

  FakeBatteries* fake_batteries_;

  if (sys_.fake_batteries_fail) {
    fake_batteries_ = new FakeBatteries(log, true, true);
  } else {
    fake_batteries_ = new FakeBatteries(log, true, false);
  }
  Data& data = Data::getInstance();

  auto state = data.getStateMachineData();
  state.current_state = State::kAccelerating;
  data.setStateMachineData(state);

  BatteryData battery_data;

  for (int i = 0; i < 50; i++) {
    fake_batteries_->getData(&battery_data);
    log.DBG("DEMO-FakeBatteries", "Voltage = %d, Current = %d, Charge = %d, Temperature = %d",
            battery_data.voltage, battery_data.current,
            battery_data.charge, battery_data.temperature);
    Thread::sleep(250);
  }
}