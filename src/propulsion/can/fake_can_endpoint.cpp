#include "fake_can_endpoint.hpp"

namespace hyped::propulsion {
FakeCanEndpoint::FakeCanEndpoint(utils::io::CanProccesor &sender) : sender_(sender)
{
}

void FakeCanEndpoint::run()
{
  utils::concurrent::Thread::sleep(1000);
  utils::io::can::Frame frame;
  sender_.processNewData(frame);
}

}  // namespace hyped::propulsion
