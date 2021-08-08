#pragma once

#include <mutex>
// lock, try_lock, unlock in mutex, unique_lock

// condition_variable[_any] -> notify_[one/all], wait

namespace hyped {
namespace utils {
namespace concurrent {

// Forward declaration
class ConditionVariable;

class Lock {
  friend ConditionVariable;

 public:
  Lock();
  ~Lock();

  /**
   * @brief      Acquire the associated lock. Blocking.
   */
  void lock();

  /**
   * @brief      Try to acquire the associated lock. Non-blocking.
   *
   * @return     True iff you own the lock now.
   */
  bool tryLock();

  /**
   * @brief      Releases the ownership of the lock. Should only be called
   *             if you own the lock.
   */
  void unlock();

 private:
  std::mutex *mutex_;
};

class ScopedLock {
 public:
  explicit ScopedLock(Lock *lock) : lock_(lock) { lock_->lock(); }
  ~ScopedLock() { lock_->unlock(); }

 private:
  Lock *lock_;
};

}  // namespace concurrent
}  // namespace utils
}  // namespace hyped
