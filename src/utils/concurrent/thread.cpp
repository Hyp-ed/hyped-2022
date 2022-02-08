#include "thread.hpp"

#include <chrono>

#include <utils/system.hpp>

namespace hyped::utils::concurrent {

namespace {

void threadEntryPoint(Thread *this_)
{
  this_->run();
}

}  // namespace

Thread::Thread(utils::Logger log) : id_(next_id_++), thread_(0), log_(log)
{
}

void Thread::start()
{
  thread_ = new std::thread(threadEntryPoint, this);
}

void Thread::join()
{
  thread_->join();
}

void Thread::run()
{
  log_.info("Empty thread started; terminating immediately");
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

}  // namespace hyped::utils::concurrent
