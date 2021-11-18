#pragma once

#include <stdint.h>

#include <array>

namespace hyped::motor_control {

using ControllerMessage = std::array<uint8_t, 8>;

static constexpr ControllerMessage kAutoAlignMessage
  // Write -4 to the modes of operation.
  = {0x2F, 0x60, 0x60, 0x00, static_cast<uint8_t>(-0x04), 0x00, 0x00, 0x00};

static constexpr ControllerMessage kCheckStateMessage
  // Check the status word in object dictionary
  = {0x40, 0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};

static constexpr std::array<ControllerMessage, 24> kConfigurationMessages = {{
  // Set the motor poles 1
  {0x2F, 0x33, 0x20, 0x00, 0x01, 0x00, 0x00, 0x00},
  // Set feedback type Hall sensor
  {0x2F, 0x40, 0x20, 0x01, 0x01, 0x00, 0x00, 0x00},
  // Set feedback direction to inverted
  {0x2B, 0x40, 0x20, 0x03, 0x01, 0x00, 0x00, 0x00},
  // Set Hall feedback config to 120 degrees
  {0x2B, 0x40, 0x20, 0x05, 0x00, 0x00, 0x00, 0x00},
  // Set feedback resolution
  {0x2B, 0x40, 0x20, 0x06, 0x06, 0x00, 0x00, 0x00},
  // Set electrical angle filter to 30
  {0x2B, 0x40, 0x20, 0x07, 0x1E, 0x00, 0x00, 0x00},
  // Set over voltage limit to 125
  {0x2B, 0x54, 0x20, 0x00, 0x7D, 0x00, 0x00, 0x00},
  // Set under voltage limit to 25
  {0x2B, 0x55, 0x20, 0x01, 0x19, 0x00, 0x00, 0x00},
  // Set under voltage minimum to 25
  {0x2B, 0x55, 0x20, 0x03, 0x19, 0x00, 0x00, 0x00},
  // Set motor temperature sensor
  {0x2F, 0x57, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00},
  // Set motor rated current to 10 000 mA
  {0x23, 0x75, 0x60, 0x00, 0x10, 0x27, 0x00, 0x00},
  // Set motor rated torque to 400 000 mN
  {0x23, 0x76, 0x60, 0x00, 0x80, 0x1A, 0x06, 0x00},
  // Set current control torque regulator P gain to 16000
  {0x2B, 0xF6, 0x60, 0x01, 0x80, 0x3E, 0x00, 0x00},
  // Set current control torque regulator I gain to 1800
  {0x2B, 0xF6, 0x60, 0x02, 0x88, 0x13, 0x00, 0x00},
  // Set current control flux regulator P gain to 2600
  {0x2B, 0xF6, 0x60, 0x03, 0x80, 0x3E, 0x00, 0x00},
  // Set current control flux regulator I gain to 1800
  {0x2B, 0xF6, 0x60, 0x04, 0x88, 0x13, 0x00, 0x00},
  // Set current control regulator ramp to 200
  {0x2B, 0xF6, 0x60, 0x05, 0xC8, 0x00, 0x00, 0x00},
  // Set maximum controller current to 170 000 mA
  {0x23, 0x50, 0x20, 0x00, 0x10, 0x98, 0x02, 0x00},
  // Set secondary controller current protection to 800 000 mA
  {0x23, 0x51, 0x20, 0x00, 0x00, 0x35, 0x0C, 0x00},
  // Set maximum velocity to 1000 RPM
  {0x23, 0x52, 0x20, 0x01, 0xE8, 0x03, 0x00, 0x00},
  // Set velocity control regulator P gain to 400
  {0x2B, 0xF9, 0x60, 0x01, 0x90, 0x01, 0x00, 0x00},
  // Set velocity control regulator I gain to 2
  {0x2B, 0xF9, 0x60, 0x02, 0x02, 0x00, 0x00, 0x00},
  // Set velocity encoder factor numerator
  {0x23, 0x94, 0x60, 0x01, 0x3C, 0x00, 0x00, 0x00},
  // Set velocity encoder factor denominator
  {0x23, 0x94, 0x60, 0x02, 0x1E, 0x00, 0x00, 0x00},
}};

static constexpr std::array<ControllerMessage, 4> kEnterOperationalMessages = {{
  // Enable velocity mode
  {0x2F, 0x60, 0x60, 0x00, 0x09, 0x00, 0x00, 0x00},
  // Apply brake
  {0x2B, 0x40, 0x60, 0x00, 0x80, 0x00, 0x00, 0x00},
  // Send shutdown message to transition from state 1 to state 2
  {0x2B, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00},
  // Send enter operational message to transition from state 2 to state 4
  {0x2B, 0x40, 0x60, 0x00, 0x0F, 0x00, 0x00, 0x00},
}};

static constexpr ControllerMessage kEnterPreOperationalMessage
  // Send shutdown command
  = {0x2B, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00};

static constexpr std::array<ControllerMessage, 2> kHealthCheckMessages = {{
  // Check warning status
  {0x40, 0x27, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00},
  // Check error status
  {0x40, 0x3F, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},
}};

static constexpr ControllerMessage kQuickStopMessage
  // Send quickStop message
  = {0x2B, 0x40, 0x60, 0x00, 0x0B, 0x00, 0x00, 0x00};

static constexpr ControllerMessage kSendTargetTorqueMessage
  // Send 32 bit integer in Little Endian bytes
  = {0x23, 0x71, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};

static constexpr ControllerMessage kSendTargetVelocityMessage =
  // Send 32 bit integer in Little Endian bytes
  {0x23, 0xFF, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};

static constexpr ControllerMessage kUpdateActualTorqueMessage
  // Check actual torque in object dictionary
  = {0x40, 0x77, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};

static constexpr ControllerMessage kUpdateActualVelocityMessage
  // Check actual velocity in object dictionary
  = {0x40, 0x6C, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};

static constexpr ControllerMessage kControllerTemperatureMessage
  // Check controller temperature in object dictionary
  = {0x40, 0x26, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00};

static constexpr ControllerMessage kMotorTemperatureMessage
  // Check motor temperature in object dictionary
  = {0x40, 0x25, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00};

}  // namespace hyped::motor_control
