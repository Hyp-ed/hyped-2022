#pragma once

#include <stdint.h>

#include <utils/io/can.hpp>

namespace hyped {

namespace propulsion {
enum ControllerState {
  kNotReadyToSwitchOn,
  kSwitchOnDisabled,
  kReadyToSwitchOn,
  kSwitchedOn,
  kOperationEnabled,
  kQuickStopActive,
  kFaultReactionActive,
  kFault,
};

class IController {
 public:
  virtual ~IController() {}
  virtual void registerController()                                                          = 0;
  virtual void configure()                                                                   = 0;
  virtual void enterOperational()                                                            = 0;
  virtual void enterPreOperational()                                                         = 0;
  virtual void checkState()                                                                  = 0;
  virtual void sendTargetVelocity(int32_t target_velocity)                                   = 0;
  virtual void updateActualVelocity()                                                        = 0;
  virtual int32_t getVelocity()                                                              = 0;
  virtual void quickStop()                                                                   = 0;
  virtual void healthCheck()                                                                 = 0;
  virtual bool getFailure()                                                                  = 0;
  virtual void updateMotorTemp()                                                             = 0;
  virtual uint8_t getMotorTemp()                                                             = 0;
  virtual ControllerState getControllerState()                                               = 0;
  virtual void processEmergencyMessage(utils::io::can::Frame &message)                       = 0;
  virtual void processErrorMessage(uint16_t error_message)                                   = 0;
  virtual void processSdoMessage(utils::io::can::Frame &message)                             = 0;
  virtual void processNmtMessage(utils::io::can::Frame &message)                             = 0;
  virtual void requestStateTransition(utils::io::can::Frame &message, ControllerState state) = 0;
};
}  // namespace propulsion
}  // namespace hyped
