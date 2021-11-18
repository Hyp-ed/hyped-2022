#pragma once

#include "condition_variable.hpp"
#include "lock.hpp"

#include <cstdint>

namespace hyped {
namespace utils {
namespace concurrent {

class Barrier {
 public:
  explicit Barrier(uint8_t required);
  ~Barrier();

  void wait();

 private:
  uint8_t required_;
  uint8_t calls_;

  Lock lock_;
  ConditionVariable cv_;
};

}  // namespace concurrent
}  // namespace utils
}  // namespace hyped
