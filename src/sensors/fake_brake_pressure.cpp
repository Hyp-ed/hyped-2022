#include "brake_pressure.hpp"

#include <stdlib.h>

#include <utils/system.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

FakeBrakePressure::FakeBrakePressure(const bool is_fail)
    : data_(data::Data::getInstance()),
      log_("FAKE-BRAKE-PRESSURE", utils::System::getSystem().config_.log_level_sensors),
      failure_(9500),
      success_(7000),
      is_fail_(is_fail),
      acc_start_time_(0),
      acc_started_(false),
      failure_time_(0),
      failure_happened_(false)
{
  pressure_data_.brake_pressure = success_;
  if (is_fail_) {
    log_.info("fail initialised");
  } else {
    log_.info("initialised");
  }
}

void FakeBrakePressure::run()
{
  // We want to fail after we start accelerating
  // We can make it random from 0 to 20 seconds
  if (!acc_started_) {
    data::State state = data_.getStateMachineData().current_state;
    if (state == data::State::kAccelerating) {
      acc_start_time_ = utils::Timer::getTimeMicros();
      // Generate a random time for a failure
      if (is_fail_) { failure_time_ = (rand() % 20 + 1) * 1000000; };
      acc_started_ = true;
    }
  }
  checkFailure();
}

void FakeBrakePressure::checkFailure()
{
  if (!is_fail_ || failure_happened_) { return; }
  const auto time_elapsed = utils::Timer::getTimeMicros() - acc_start_time_;
  if (time_elapsed >= failure_time_) {
    pressure_data_.brake_pressure = failure_;
    failure_happened_             = true;
  }
}

uint8_t FakeBrakePressure::getData() const
{
  return pressure_data_.brake_pressure;
}

}  // namespace hyped::sensors
