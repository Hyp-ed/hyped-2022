/*
 * Authors: M. Kristien
 * Organisation: HYPED
 * Date: Dec 2019
 * Description:
 *
 *    Copyright 2019 HYPED
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

#include <stdio.h>

#include "demo/interface.hpp"
#include "utils/interface_factory.hpp"

namespace hyped {
namespace demo {

class Implementation1 : public DemoInterface {
 public:
  Implementation1() {}

  void printYourName() override
  {
    printf("hello, my name is Implementation1\n");
  }
};

namespace {
DemoInterface* createImplementation1()
{
  return new Implementation1();
}
bool reg_impl = utils::InterfaceFactory<DemoInterface>
                ::registerCreator("Implementation1", createImplementation1);
}


}}  // namespace hyped::demo
