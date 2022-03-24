#include "fake_temperature.hpp"

#include <stdlib.h>

#include <utils/system.hpp>
#include <utils/timer.hpp>

namespace hyped::sensors {

FakeTemperature::FakeTemperature(const bool is_fail)
    : data_(data::Data::getInstance()),
      log_("FAKE-TEMPERATURE", utils::System::getSystem().config_.log_level_sensors),
      failure_(300),
      success_(30),
      is_fail_(is_fail),
      acc_start_time_(0),
      acc_started_(false),
      failure_time_(0),
      failure_happened_(false)
{
  temperature_data_.temperature = success_;
  if (is_fail_) {
    log_.info("fail initialised");
  } else {
    log_.info("initialised");
  }
}

void FakeTemperature::run()
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

void FakeTemperature::checkFailure()
{
  if (is_fail_ && failure_time_ != 0 && !failure_happened_) {
    if (utils::Timer::getTimeMicros() - acc_start_time_ >= failure_time_) {
      temperature_data_.temperature = failure_;
      failure_happened_             = true;
    }
  }
}

uint8_t FakeTemperature::getData()
{
  return temperature_data_.temperature;
}

}  // namespace hyped::sensors
