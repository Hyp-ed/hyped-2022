#include "lock.hpp"

namespace hyped {
namespace utils {
namespace concurrent {

Lock::Lock()
{
  mutex_ = new std::mutex();
}

Lock::~Lock()
{
  delete mutex_;
}

void Lock::lock()
{
  mutex_->lock();
}

bool Lock::tryLock()
{
  return mutex_->try_lock();
}

void Lock::unlock()
{
  return mutex_->unlock();
}

}  // namespace concurrent
}  // namespace utils
}  // namespace hyped
