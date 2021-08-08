#include "thread.hpp"

#include <chrono>
#include <utils/system.hpp>

namespace hyped {
namespace utils {
namespace concurrent {

namespace {

void thread_entry_point(Thread *this_)
{
  this_->run();
}

}  // namespace

Thread::Thread(Logger &log) : id_(-1), thread_(0), log_(log)
{ /* EMPTY */
}

Thread::Thread(uint8_t id) : id_(id), thread_(0), log_(System::getLogger())
{ /* EMPTY */
}

Thread::Thread() : id_(-1), thread_(0), log_(System::getLogger())
{ /* EMPTY */
}

Thread::Thread(uint8_t id, Logger &log) : id_(id), thread_(0), log_(log)
{ /* EMPTY */
}

Thread::~Thread()
{ /* EMPTY */
}

void Thread::start()
{
  thread_ = new std::thread(thread_entry_point, this);
}

void Thread::join()
{
  thread_->join();
}

void Thread::run()
{
  log_.INFO("THREAD", "You are starting EMPTY thread. Terminating now.");
}

void Thread::yield()
{
  std::this_thread::yield();
}

void Thread::sleep(uint32_t ms)
{
  std::this_thread::sleep_for(std::chrono::microseconds(ms * 1000));
}

void BusyThread::run()
{
  uint64_t i = 0;
  while (running_) {
    i++;
    if (i % 10000 == 0) {
      // log_.INFO("BUSY", "output\n");
    }
  }
}

BusyThread::~BusyThread()
{
  running_ = false;
}

}  // namespace concurrent
}  // namespace utils
}  // namespace hyped
