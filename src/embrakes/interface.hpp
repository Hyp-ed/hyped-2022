#pragma once

namespace hyped {

namespace embrakes {

class StepperInterface {
 public:
  virtual void checkHome()           = 0;
  virtual void sendRetract()         = 0;
  virtual void sendClamp()           = 0;
  virtual void checkAccFailure()     = 0;
  virtual void checkBrakingFailure() = 0;
  virtual bool checkClamped()        = 0;

  // Explicit virtual deconstructor needs to be declared *and* defined
  virtual ~StepperInterface() {}
};

}  // namespace embrakes
}  // namespace hyped
