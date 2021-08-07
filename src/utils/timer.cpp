#include "timer.hpp"

#include <sys/time.h>

namespace hyped {
namespace utils {

uint64_t Timer::time_start_ = Timer::getTimeMicros();

// uint64_t Timer::getTimeMillis()
// {
//   return getTimeMicros() / 1000;
// }

uint64_t Timer::getTimeMicros()
{
  timeval tv;
  if (gettimeofday(&tv, (struct timezone *)0) < 0) { return 0; }
  return (static_cast<uint64_t>(tv.tv_sec) * 1000000) + tv.tv_usec - time_start_;
}

Timer::Timer() : elapsed_(0), start_(0), stop_(0)
{ /* EMPTY */
}

void Timer::start()
{
  start_ = getTimeMicros();
  stop_  = 0;
}

void Timer::stop()
{
  stop_ = getTimeMicros();
  elapsed_ += stop_ - start_;
}

void Timer::reset()
{
  start_   = 0;
  stop_    = 0;
  elapsed_ = 0;
}

uint64_t Timer::getMicros() const
{
  return elapsed_;
}

double Timer::getMillis() const
{
  return static_cast<double>(elapsed_) * 1.0e-3;
}

double Timer::getSeconds() const
{
  return static_cast<double>(elapsed_) * 1.0e-6;
}

}  // namespace utils
}  // namespace hyped
