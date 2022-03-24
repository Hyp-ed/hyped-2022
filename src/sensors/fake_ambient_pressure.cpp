#include "fake_ambient_pressure.hpp"

#include <stdlib.h>

#include <utils/system.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

FakeAmbientPressure::FakeAmbientPressure(const bool is_fail)
    : data_(data::Data::getInstance()),
      log_("FAKE-AMBIENT-PRESSURE", utils::System::getSystem().config_.log_level_sensors),
      failure_(10000),  // 10 bar
      success_(1000),   // 1 bar
      is_fail_(is_fail),
      acc_start_time_(0),
      acc_started_(false),
      failure_time_(0),
      failure_happened_(false)
{
  pressure_data_.ambient_pressure = success_;
  if (is_fail_) {
    log_.info("fail initialised");
  } else {
    log_.info("initialised");
  }
}

void FakeAmbientPressure::run()
{
  // We want to fail after we start accelerating
  // We can make it random from 0 to 20 seconds
  if (!acc_started_) {
    data::State state = data_.getStateMachineData().current_state;
    if (state == data::State::kAccelerating) {
      acc_start_time_ = utils::Timer::getTimeMicros();
      // Generate a random time for a failure
      if (is_fail_) failure_time_ = (rand() % 20 + 1) * 1000000;
      acc_started_ = true;
    }
  }
  checkFailure();
}

void FakeAmbientPressure::checkFailure()
{
  if (is_fail_ && failure_time_ != 0 && !failure_happened_) {
    if (utils::Timer::getTimeMicros() - acc_start_time_ >= failure_time_) {
      pressure_data_.ambient_pressure = failure_;
      failure_happened_               = true;
    }
  }
}

uint16_t FakeAmbientPressure::getData()
{
  return pressure_data_.ambient_pressure;
}

}  // namespace hyped::sensors
