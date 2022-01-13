#include "fake_can_endpoint.hpp"

namespace hyped {
namespace propulsion {
FakeCanEndpoint::FakeCanEndpoint(CanProccesor *sender) : sender_(sender)
{
}

void FakeCanEndpoint::run()
{
  sleep(1000);
  Frame frame;
  sender_->processNewData(frame);
}
}  // namespace propulsion
}  // namespace hyped
