/*
 * Authors: M. Kristien
 * Organisation: HYPED
 * Date: 28. February 2018
 * Description:
 *
 *    Copyright 2018 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef UTILS_CONCURRENT_BARRIER_HPP_
#define UTILS_CONCURRENT_BARRIER_HPP_

#include <cstdint>

#include "condition_variable.hpp"
#include "lock.hpp"

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

#endif  // UTILS_CONCURRENT_BARRIER_HPP_
