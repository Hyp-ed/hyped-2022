#include "barrier.hpp"

namespace hyped {
namespace utils {
namespace concurrent {

Barrier::Barrier(uint8_t required) : required_(required), calls_(0)
{ /* EMPTY */
}

Barrier::~Barrier()
{ /* EMPTY */
}

void Barrier::wait()
{
  ScopedLock L(&lock_);
  calls_++;

  if (calls_ != required_) {
    cv_.wait(&lock_);
  } else {
    calls_ = 0;
    cv_.notifyAll();
  }
}

}  // namespace concurrent
}  // namespace utils
}  // namespace hyped
