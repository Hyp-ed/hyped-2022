/*
 * Author: Atte Niemi
 * Organisation: HYPED
 * Date:
 * Description: Main brake interfaces used to create fake brakes
 *
 *    Copyright 2020 HYPED
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

#ifndef EMBRAKES_INTERFACE_HPP_
#define EMBRAKES_INTERFACE_HPP_

namespace hyped {

namespace embrakes {

class StepperInterface {
 public:
  virtual void checkHome() = 0;
  virtual void sendRetract() = 0;
  virtual void sendClamp() = 0;
  virtual void checkAccFailure() = 0;
  virtual void checkBrakingFailure() = 0;
  virtual bool checkClamped() = 0;
};

}  // namespace embrakes
}  // namespace hyped
#endif  // EMBRAKES_INTERFACE_HPP_
