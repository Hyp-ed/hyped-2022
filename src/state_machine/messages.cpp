#include "messages.hpp"

namespace hyped {

namespace state_machine {

const char Messages::kStmLoggingIdentifier[] = "STM";

//--------------------------------------------------------------------------------------
// Emergency
//--------------------------------------------------------------------------------------

const char Messages::kStopCommandLog[] = "STOP command received";

const char Messages::kCriticalNavigationLog[] = "Critical failure in navigation";

const char Messages::kCriticalTelemetryLog[] = "Critical failure in telemetry";

const char Messages::kCriticalMotorsLog[] = "Critical failure in motors";

const char Messages::kCriticalEmbrakesLog[] = "Critical failure in embrakes";

const char Messages::kCriticalBatteriesLog[] = "Critical failure in batteries";

const char Messages::kCriticalSensorsLog[] = "Critical failure in sensors";

//--------------------------------------------------------------------------------------
// Module Status
//--------------------------------------------------------------------------------------

const char Messages::kCalibrateInitialisedLog[]
  = "Calibrate command received and all modules initialised";

const char Messages::kModulesCalibratedLog[] = "All modules calibrated";

//--------------------------------------------------------------------------------------
// Navigation Data Events
//--------------------------------------------------------------------------------------

const char Messages::kBrakingZoneLog[] = "Entered braking zone";

const char Messages::kMaxVelocityLog[] = "Reached maximum velocity";

const char Messages::kPodStoppedLog[] = "The pod has stopped";

//--------------------------------------------------------------------------------------
// State Transitions
//--------------------------------------------------------------------------------------

const char Messages::kShutdownLog[] = "Shutting down";

const char Messages::kEnteringStateFormat[] = "Entering %s state";

const char Messages::kExitingStateFormat[] = "Exiting %s state";

const char Messages::kExitingProgramFormat[] = "Exiting. Current state: %s";

const char Messages::kTransitionFromOffLog[] = "Tried to transition from Off state";

}  // namespace state_machine

}  // namespace hyped
