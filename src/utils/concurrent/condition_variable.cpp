#include "condition_variable.hpp"

#include "lock.hpp"

namespace hyped {
namespace utils {
namespace concurrent {

ConditionVariable::ConditionVariable()
{
  cond_var_ = new std::CV();
}

ConditionVariable::~ConditionVariable()
{
  delete cond_var_;
}

void ConditionVariable::notify()
{
  cond_var_->notify_one();
}

void ConditionVariable::notifyAll()
{
  cond_var_->notify_all();
}

void ConditionVariable::wait(Lock *lock)
{
  cond_var_->wait(*lock->mutex_);
}

}  // namespace concurrent
}  // namespace utils
}  // namespace hyped
