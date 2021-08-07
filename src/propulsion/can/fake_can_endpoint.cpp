#include "fake_can_endpoint.hpp"

namespace hyped {
namespace motor_control {
FakeCanEndpoint::FakeCanEndpoint(CanProccesor *sender) : sender_(sender)
{
}

void FakeCanEndpoint::run()
{
  sleep(1000);
  Frame frame;
  sender_->processNewData(frame);
}
}  // namespace motor_control
}  // namespace hyped
