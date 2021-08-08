#pragma once

#define CV condition_variable_any

#include <condition_variable>

namespace hyped {
namespace utils {
namespace concurrent {

// Forward declaration
class Lock;

class ConditionVariable {
 public:
  ConditionVariable();
  ~ConditionVariable();

  /**
   * @brief      Wake up one thread waiting for this CV.
   */
  void notify();

  /**
   * @brief      Wake up all threads waiting for this CV.
   */
  void notifyAll();

  /**
   * @brief      Block current thread until this CV is signalled/notified.
   *
   * @param      lock  The lock associated with this CV. The lock is unlocked
   *                   upon calling this method and reacquired just before returning.
   *                   Note, the lock must be acquired by the caller before calling wait().
   */
  void wait(Lock *lock);

 private:
  std::CV *cond_var_;
};

}  // namespace concurrent
}  // namespace utils
}  // namespace hyped
