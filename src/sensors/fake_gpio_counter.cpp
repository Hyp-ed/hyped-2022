#include "fake_gpio_counter.hpp"

#include <cmath>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <utils/timer.hpp>

uint64_t kBrakeTime     = 10000000;
uint32_t kTrackDistance = 2000;
double kStripeDistance  = 30.48;    // metres
uint64_t kMaxTime       = 1500000;  // between stripe readings before throw failure (micros)

namespace hyped::sensors {

FakeGpioCounter::FakeGpioCounter(utils::Logger &log, const bool miss_stripe)
    : log_(log),
      data_(data::Data::getInstance()),
      miss_stripe_(miss_stripe),
      is_from_file_(false),
      acc_ref_init_(false),
      stripe_file_timestamp_(0)

{
  stripe_counter_.count.value = 0;  // start stripe count
  stripe_counter_.operational = true;
}

FakeGpioCounter::FakeGpioCounter(utils::Logger &log, const bool miss_stripe,
                                 const std::string file_path)
    : log_(log),
      data_(data::Data::getInstance()),
      miss_stripe_(miss_stripe),
      is_from_file_(true),
      acc_ref_init_(false),
      stripe_file_timestamp_(0)
{
  stripe_counter_.count.value     = 0;  // start stripe count
  stripe_counter_.operational     = true;
  stripe_counter_.count.timestamp = 0;
  readFromFile(file_path);  // read text from file into vector class member
  if (miss_stripe_) {
    log_.INFO("Fake-GpioCounter", "Fake Keyence Fail initialised");
  } else {
    log_.INFO("Fake-GpioCounter", "Fake Keyence initialised");
  }
}

void FakeGpioCounter::getData(data::CounterData &stripe_count)  // returns incorrect stripe count
{
  data::State state = data_.getStateMachineData().current_state;
  if (!acc_ref_init_ && state == data::State::kAccelerating) {
    accel_start_time_ = utils::Timer::getTimeMicros();
    acc_ref_init_     = true;
  }

  if (is_from_file_) {
    // Get time in micro seconds and iterate through the vector until we find what stripe we are at
    if ((state == data::State::kAccelerating || state == data::State::kNominalBraking
         || state == data::State::kCruising)
        && acc_ref_init_) {
      uint64_t time_now_micro = (utils::Timer::getTimeMicros() - accel_start_time_);

      for (data::CounterData stripe : stripe_data_) {
        if (stripe.count.timestamp < time_now_micro) {
          stripe_counter_.count.value = stripe.count.value;
          // use system timestamp from file
          stripe_counter_.count.timestamp = utils::Timer::getTimeMicros();
          stripe_file_timestamp_          = stripe.count.timestamp;
        } else {
          break;
        }
      }
    }
    checkData();
  } else {
    // We are not in a state were we have data from a text file
    // base data of the navigation output
    data::Navigation nav   = data_.getNavigationData();  // throw failure from fake_imu
    uint32_t current_count = stripe_counter_.count.value;

    uint16_t nav_count = std::floor(nav.displacement / kStripeDistance);  // cast floor int;

    if (current_count != nav_count) {
      stripe_counter_.count.value     = nav_count;
      stripe_counter_.count.timestamp = utils::Timer::getTimeMicros();
    }
  }
  stripe_count = stripe_counter_;
}

void FakeGpioCounter::checkData()
{
  if (is_from_file_) {
    uint64_t time_after = 0;
    if (acc_ref_init_) {
      time_after = utils::Timer::getTimeMicros() - (stripe_file_timestamp_ + accel_start_time_);
    }
    log_.DBG3("Fake-GpioCounter", "time_after: %d", time_after);
    if (time_after > kMaxTime && miss_stripe_
        && stripe_counter_.count.value
             > 1) {  // time_after is longer on first few stripes NOLINT [whitespace/line_length]
      log_.INFO("Fake-GpioCounter", "missed stripe!");
      stripe_counter_.operational = false;
    }
  }
}

void FakeGpioCounter::readFromFile(const std::string file_path)
{
  std::ifstream data_file(file_path, std::ifstream::in);
  float count;
  float time;
  if (data_file.is_open()) {
    // read in pairs of stripe_count, timestamp
    while (data_file >> time && data_file >> count) {
      data::CounterData this_line;
      this_line.count.value = count;
      // save timestamps in Microseconds
      this_line.count.timestamp = time * 1000;
      stripe_data_.push_back(this_line);
    }
  } else {
    log_.ERR("Fake-GpioCounter", "cannot open file");
  }
  data_file.close();
}

bool FakeGpioCounter::isOnline()
{
  return stripe_counter_.operational;
}
}  // namespace hyped::sensors
